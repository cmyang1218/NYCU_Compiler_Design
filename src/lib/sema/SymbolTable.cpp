#include "sema/SymbolTable.hpp"

extern uint32_t opt_dum;

SymbolEntry::SymbolEntry(const char *name, const char *kind, 
                            int level, const char *type, const char *attr, 
                            const Location &loc, bool status) : 
    _name{name}, _kind{kind}, _level{level}, 
        _type{type}, _attr{attr}, _loc{loc}, _status{status} {}

std::string SymbolEntry::getName() {
    return this->_name;
}

std::string SymbolEntry::getKind() {
    return this->_kind;
}

int SymbolEntry::getLevel() {
    return this->_level;
}

std::string SymbolEntry::getType() {
    return this->_type;
}

std::string SymbolEntry::getAttribute() {
    return this->_attr;
}

Location SymbolEntry::getLocation() {
    return this->_loc;
}

bool SymbolEntry::getStatus() {
    return this->_status;
}

void SymbolEntry::setLocalPos(int pos) {
    this->_localpos = pos;
    return;
}

int SymbolEntry::getLocalPos() {
    return this->_localpos;
}

SymbolTable::SymbolTable(int type) {
    this->_type = type;
}


void SymbolTable::insertSymbolEntry(SymbolEntry entry) {
    this->_entries.push_back(entry);
    return;
}

void SymbolTable::dumpDemarcation(const char chr) {
    for (size_t i = 0; i < 110; ++i) {
        printf("%c", chr);
    }
    puts("");
    return;
}

void SymbolTable::dumpSymbol() {
    dumpDemarcation('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    dumpDemarcation('-');
    for(auto& entry : this->_entries) {
        printf("%-33s", entry.getName().c_str());
        printf("%-11s", entry.getKind().c_str());
        if(entry.getLevel() == 0) {
            printf("%d%-10s", entry.getLevel(), "(global)");
        }else {
            printf("%d%-10s", entry.getLevel(), "(local)");
        }
        printf("%-17s", entry.getType().c_str());
        printf("%-11s", entry.getAttribute().c_str());
        puts("");
    }
    dumpDemarcation('-');
    return;
}

int SymbolTable::getTableType() {
    return this->_type;
}

void SymbolTable::setTableType(int type) {
    this->_type = type;
    return;
}

std::vector<SymbolEntry> SymbolTable::getEntries() {
    return this->_entries;
}

void SymbolManager::pushScope(SymbolTable *new_scope) {
    this->_tables.push(new_scope);
    this->_iterable_tables.push_back(new_scope);
    this->increaseLevel();
    return;
}

void SymbolManager::popScope() {
    SymbolTable* table = this->_tables.top();
    this->_tables.pop();
    this->_iterable_tables.pop_back();
    if(opt_dum)
        table->dumpSymbol();
    this->decreaseLevel();
    return;
}

SymbolTable* SymbolManager::getCurrentTable() {
    return this->_tables.top();
}

void SymbolManager::pushProcedure(std::string ret_type) {
    this->_proc_tables.push(ret_type);
    return;
}

void SymbolManager::popProcedure() {
    this->_proc_tables.pop();
    return;
}

std::string SymbolManager::getCurrentProcTable() {
    return this->_proc_tables.top();
}

void SymbolManager::insertEntryToTable(SymbolEntry new_entry) {
    SymbolTable *curr_table = this->getCurrentTable();
    std::vector<SymbolEntry> entries = curr_table->getEntries();
    bool redeclare_error = false, loop_redeclare_error = false;
    for(auto &entry : entries) {
        if(entry.getName() == new_entry.getName()) {
            redeclare_error = true;
            break;
        }
    }
    for(auto &loop_var : this->_locked_variables) {
        if(loop_var == new_entry.getName()) {
            loop_redeclare_error = true;
            break;
        }
    }
    if(redeclare_error || loop_redeclare_error) {
        Location error_loc = new_entry.getLocation();
        std::string error_msg = "symbol '" + new_entry.getName() + "' is redeclared\n";
        this->checker.appendErrorMessages(error_msg);
        this->checker.appendErrorLocations(error_loc);
        return;
    }
    curr_table->insertSymbolEntry(new_entry);
    return;
}


void SymbolManager::increaseLevel() {
    this->_level++;
    return;
}

void SymbolManager::decreaseLevel() {
    this->_level--;
    return;
}

int SymbolManager::getLevel() {
    return this->_level;
}

void SymbolManager::lockLoopVariables(std::string loop_variable_name) {
    this->_locked_variables.push_back(loop_variable_name);
    return;
}

void SymbolManager::unlockLoopVariables() {
    this->_locked_variables.pop_back();
    return;
}

std::vector<std::string> SymbolManager::getLockedLoopVariables() {
    return this->_locked_variables;
}

std::vector<SymbolTable *> SymbolManager::getIterableTables() {
    return this->_iterable_tables;
}

std::vector<int> SymbolManager::getArrayIndices(std::string type) {
    std::vector<int> indices;
    std::size_t idx_pos = type.find(" ");
    if(idx_pos != std::string::npos) {
        std::string arr_dim = type.substr(idx_pos+1);
        std::regex pattern("\\[\\d+\\]");
        auto match_start = std::sregex_iterator(arr_dim.begin(), arr_dim.end(), pattern);
        auto match_end = std::sregex_iterator();
        for(auto it = match_start; it != match_end; ++it) {
            std::smatch match = *it;
            const char *matched_idx = match.str().c_str();
            char *arr_idx = strndup(matched_idx+1, strlen(matched_idx)-2);
            indices.push_back(atoi(arr_idx));
        }
    }
    return indices;
}

void SymbolManager::reconstructHashTableFromSymbolTable(SymbolTable *table) {
    if(!table)
        return;
    std::vector<SymbolEntry> entries = table->getEntries();
    for(auto &entry : entries) {
        this->_hash_entries[entry.getName()] = &entry;
    }
    return;
}

void SymbolManager::removeSymbolsFromHashTable(SymbolTable *table) {
    if(!table) 
        return;
    std::vector<SymbolEntry> entries = table->getEntries();
    for(auto &entry : entries) {
        this->_hash_entries.erase(entry.getName());
    }
    return;
}

SymbolEntry* SymbolManager::lookupFromHashTable(std::string symbol) {
    return this->_hash_entries[symbol];
}