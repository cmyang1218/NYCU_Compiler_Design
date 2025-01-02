#ifndef SEMA_SYMBOL_TABLE_H
#define SEMA_SYMBOL_TABLE_H
#include "AST/ast.hpp"
#include "sema/SemanticChecker.hpp"
#include <vector>
#include <stack>
#include <string>
#include <regex>
class SymbolEntry {
private:
    std::string _name;
    std::string _kind;
    int _level;
    std::string _type;
    std::string _attr;
    const Location& _loc;
    bool _status;
    int _localpos = 0;
public:
    
    SymbolEntry(const char *name, const char *kind, int level,
                    const char *type, const char *attr, const Location &loc, bool status);
    std::string getName();
    std::string getKind();
    int getLevel();
    std::string getType();
    std::string getAttribute();
    Location getLocation();
    bool getStatus();
    void setLocalPos(int pos);
    int getLocalPos();
};

class SymbolTable {
private:
    std::vector<SymbolEntry> _entries;
    int _type;

public:
    SymbolTable(int type);
    void dumpDemarcation(const char chr);
    void dumpSymbol();
    int getTableType();
    void setTableType(int type);
    std::vector<SymbolEntry> getEntries();
    void insertSymbolEntry(SymbolEntry entry);
};

class SymbolManager {
private:
    int _level = -1;
    std::stack<SymbolTable *> _tables;
    std::vector<SymbolTable *> _iterable_tables;
    std::stack<std::string> _proc_tables;
    std::vector<std::string> _locked_variables;
    std::map<std::string, SymbolEntry *> _hash_entries;
public:
    SemanticChecker checker;    
    
    void pushScope(SymbolTable *new_scope);
    void popScope();
    SymbolTable* getCurrentTable();
    void pushProcedure(std::string ret_type);
    void popProcedure();
    std::string getCurrentProcTable();
    void insertEntryToTable(SymbolEntry entry);
    void increaseLevel();
    void decreaseLevel();
    int getLevel();
    void lockLoopVariables(std::string loop_variable_name);
    void unlockLoopVariables();
    std::vector<std::string> getLockedLoopVariables();
    std::vector<SymbolTable *> getIterableTables();
    std::vector<int> getArrayIndices(std::string type);
    void reconstructHashTableFromSymbolTable(SymbolTable *table);
    void removeSymbolsFromHashTable(SymbolTable* table);
    SymbolEntry* lookupFromHashTable(std::string symbol);
};

#endif