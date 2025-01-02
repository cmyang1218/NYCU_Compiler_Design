#ifndef SEMA_SEMANTIC_CHECKER_H
#define SEMA_SEMANTIC_CHECKER_H
#include "AST/ast.hpp"
#include <vector>
#include <string>

class SemanticChecker {
private:
    std::vector<std::string> error_messages;
    std::vector<Location> error_locations;
public:
    std::vector<std::string> getErrorMessages();
    void appendErrorMessages(std::string error_msg);
    std::vector<Location> getErrorLocations();
    void appendErrorLocations(Location loc);
};
#endif