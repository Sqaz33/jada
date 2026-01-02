#pragma once

#include "isemantics_part.hpp"

namespace semantics_part {

class EntryPointCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class ModuleNameCheck : public ISemanticsPart { 
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class OneLevelWithCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class SelfImportCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

} // namespace semantics_part