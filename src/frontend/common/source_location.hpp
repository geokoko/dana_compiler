#pragma once

struct SourceLoc {
    int line = 0;
    int col = 0;

	static SourceLoc builtin() {
		return SourceLoc{-1, -1};
	} // default-dummy location for builtins
};
