#include "sema/SemanticChecker.hpp"
#include "visitor/AstNodeInclude.hpp"

std::vector<std::string> SemanticChecker::getErrorMessages() {
    return this->error_messages;
}

void SemanticChecker::appendErrorMessages(std::string error_msg) {
    this->error_messages.push_back(error_msg);
    return;
}

std::vector<Location> SemanticChecker::getErrorLocations() {
    return this->error_locations;
}

void SemanticChecker::appendErrorLocations(Location loc) {
    this->error_locations.push_back(loc);
    return;
}
