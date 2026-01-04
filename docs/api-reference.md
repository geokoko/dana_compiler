# Dana Compiler API Reference

This document describes the internal APIs of the Dana compiler.

## AST Classes

### Base Classes

#### AstNode

```cpp
class AstNode {
public:
    SourceLoc loc;                    // Source location
    virtual void accept(AstVisitor&); // Visitor pattern
    virtual void print(std::ostream&, int indent = 0);
};
```

#### AstVisitor

```cpp
class AstVisitor {
public:
    virtual void visit(Type& n) = 0;
    virtual void visit(FParType& n) = 0;
    virtual void visit(Program& n) = 0;
    virtual void visit(FParDef& n) = 0;
    virtual void visit(Header& n) = 0;
    virtual void visit(VarDef& n) = 0;
    virtual void visit(FuncDecl& n) = 0;
    virtual void visit(FuncDef& n) = 0;
    virtual void visit(Block& n) = 0;
    // ... all statement types
    // ... all expression types
    // ... all condition types
};
```

### Type Nodes

#### Type

Represents a type declaration in source code.

```cpp
class Type : public AstNode {
public:
    DataType data_type() const;          // INT or BYTE
    const vec<optional<int>>& dimensions() const;
};
```

#### FParType

Represents a parameter type (may have pass-by-reference).

```cpp
class FParType : public Type {
public:
    bool isByRef() const;
};
```

### Definition Nodes

#### VarDef

```cpp
class VarDef : public Def {
public:
    const vec<std::string>& identifiers() const;
    Type* declaredType() const;
    vec<VarSymbol*>& symbols();  // Set by semantic analysis
};
```

#### FuncDef

```cpp
class FuncDef : public Def {
public:
    Header* funcHeader() const;
    vec<up<Def>>& localDefs();
    Block* funcBody() const;
    bool isEntrypoint() const;
    void setEntrypoint(bool);
};
```

#### FuncDecl

```cpp
class FuncDecl : public Def {
public:
    Header* funcHeader() const;
};
```

### Statement Nodes

#### AssignStmt

```cpp
class AssignStmt : public Stmt {
public:
    Lval* left() const;
    Expr* right() const;
};
```

#### IfStmt

```cpp
class IfStmt : public Stmt {
public:
    Cond* conditionExpr() const;
    Block* thenBlock() const;
    vec<pair<up<Cond>, up<Block>>>& elifs();
    Block* elseBlock() const;
};
```

#### LoopStmt

```cpp
class LoopStmt : public Stmt {
public:
    const optional<string>& loopLabel() const;
    Block* loopBody() const;
};
```

### Expression Nodes

#### Expr Base

```cpp
class Expr : public AstNode {
public:
    SemaTypePtr type() const;           // Computed type
    void setType(SemaTypePtr);
    bool isLValue() const;
    void setLValue(bool);
    bool isAssignable() const;
    void setAssignable(bool);
    bool isConstExpr() const;
    void setConstExpr(bool);
};
```

#### FuncCall

```cpp
class FuncCall : public Expr {
public:
    const string& identifier() const;
    vec<up<Expr>>& arguments();
    FuncSymbol* funcSymbol() const;     // Set by semantic analysis
    void setFuncSymbol(FuncSymbol*);
};
```

### Condition Nodes

#### RelCond

```cpp
class RelCond : public Cond {
public:
    RelOp opKind() const;    // Eq, Ne, Lt, Gt, Le, Ge
    Expr* leftExpr() const;
    Expr* rightExpr() const;
};
```

#### BinaryCond

```cpp
class BinaryCond : public Cond {
public:
    LogicOp opKind() const;  // And, Or
    Cond* leftCond() const;
    Cond* rightCond() const;
};
```

## Symbol System

### Symbol Base

```cpp
class Symbol {
public:
    enum class SymKind { VAR, PARAM, FUNC };
    enum class ParamPass { BY_VAL, BY_REF };
    
    const string& getName() const;
    SemaTypePtr getType() const;
    SymKind getKind() const;
    const SourceLoc& getLocation() const;
    
    bool isVariable() const;
    bool isParameter() const;
    bool isFunction() const;
    bool isDefined() const;
    void markDefined();
};
```

### VarSymbol

```cpp
class VarSymbol : public Symbol {
public:
    VarSymbol(const string& name, SemaTypePtr type, const SourceLoc& loc);
    FuncSymbol* getDefiningFunc() const;
    void setDefiningFunc(FuncSymbol*);
};
```

### ParamSymbol

```cpp
class ParamSymbol : public Symbol {
public:
    ParamSymbol(const string& name, SemaTypePtr type, ParamPass pass, const SourceLoc& loc);
    ParamPass getPass() const;
};
```

### FuncSymbol

```cpp
class FuncSymbol : public Symbol {
public:
    FuncSymbol(const string& name, SemaTypePtr sig, bool isProcedure, const SourceLoc& loc);
    bool isProcedure() const;
    const vector<ParamSymbol*>& getParams() const;
    void addParam(ParamSymbol* param);
    void clearParams();
    void markForwardDeclaration();
    bool isForwardDeclared() const;
};
```

## Type System

### SemaType Base

```cpp
class SemaType {
public:
    enum class TypeKind { INT, BYTE, VOID, ARRAY, FUNC };
    
    TypeKind getKind() const;
    virtual bool equals(const SemaType& other) const = 0;
};

using SemaTypePtr = shared_ptr<const SemaType>;
```

### Type Constructors

```cpp
// Create singleton types
SemaTypePtr makeIntType();
SemaTypePtr makeByteType();
SemaTypePtr makeVoidType();

// Create composite types
SemaTypePtr makeArrayType(SemaTypePtr elementType, optional<size_t> size);
SemaTypePtr makeFuncType(SemaTypePtr returnType, vector<SemaTypePtr> paramTypes);
```

### ArrayType

```cpp
class ArrayType : public SemaType {
public:
    SemaTypePtr elementType() const;
    optional<size_t> size() const;  // nullopt for unsized
};
```

### FuncType

```cpp
class FuncType : public SemaType {
public:
    SemaTypePtr returnType() const;
    const vector<SemaTypePtr>& paramTypes() const;
};
```

## Semantic Analysis

### SemContext

```cpp
class SemContext {
public:
    struct FunctionFrame {
        FuncSymbol* symbol;
        bool isProcedure;
        SemaTypePtr returnType;
    };
    
    SemContext(SymbolTable& symtab, Diagnostics& diags);
    
    // Scope management
    void openScope();
    void closeScope();
    
    // Symbol operations
    struct LookupResult { Symbol* symbol; int scopeLevel; };
    LookupResult lookupSymbol(const string& name);
    LookupResult lookupLocalSymbol(const string& name);
    LookupResult declareSymbol(unique_ptr<Symbol> sym);
    
    // Function context
    void enterFunction(FunctionFrame& frame);
    void leaveFunction();
    FunctionFrame* currentFunction();
    
    // Diagnostics
    Diagnostics& diags();
    bool hasErrors() const;
};
```

### SemanticPass

```cpp
class SemanticPass : public AstVisitor {
public:
    explicit SemanticPass(SemContext& context);
    
    // Type utilities
    static bool isIntType(const SemaTypePtr& t);
    static bool isByteType(const SemaTypePtr& t);
    static bool isArrayType(const SemaTypePtr& t);
    static bool typesEqual(const SemaTypePtr& a, const SemaTypePtr& b);
    static bool typesCompatible(const SemaTypePtr& actual, const SemaTypePtr& expected);
};

void runSemanticPass(Program& program, SemContext& context);
```

### ControlFlowPass

```cpp
class ControlFlowPass : public AstVisitor {
public:
    explicit ControlFlowPass(SemContext& context);
};

void runControlFlowPass(Program& program, SemContext& context);
```

## Code Generation

### CodegenContext

```cpp
class CodegenContext {
public:
    CodegenContext(const string& moduleName);
    
    llvm::LLVMContext& llvmContext();
    llvm::Module& llvmModule();
    llvm::IRBuilder<>& builder();
    
    // Value mapping
    void mapSymbol(Symbol* sym, llvm::Value* val);
    llvm::Value* lookupSymbol(Symbol* sym);
    
    // Function context
    void enterFunction(llvm::Function* func, FuncSymbol* sym);
    void leaveFunction();
    llvm::Function* currentFunction();
    
    // Loop context (for break/continue)
    void pushLoop(llvm::BasicBlock* breakTarget, llvm::BasicBlock* continueTarget,
                  const optional<string>& label);
    void popLoop();
    pair<llvm::BasicBlock*, llvm::BasicBlock*> getLoopTargets(const optional<string>& label);
};
```

### Codegen

```cpp
class Codegen : public AstVisitor {
public:
    explicit Codegen(CodegenContext& ctx);
    
    // Expression evaluation returns llvm::Value*
    llvm::Value* lastValue() const;
};
```

## Diagnostics

```cpp
class Diagnostics {
public:
    enum class Severity { Error, Warning, Note };
    enum class Phase { Lexer, Parser, Semantic };
    
    void setFilename(const string& filename);
    
    void report(Severity sev, Phase phase, const SourceLoc& loc, const string& msg);
    void report(Severity sev, int line, int col, const char* fmt, ...);
    
    void printAll();
    bool hasErrors() const;
};
```

## Utility Types

### Source Location

```cpp
struct SourceLoc {
    int line;
    int col;
    
    static SourceLoc builtin();  // For built-in symbols
};
```

### Common Aliases

```cpp
template<class T> using up = std::unique_ptr<T>;
template<class T> using vec = std::vector<T>;
```
