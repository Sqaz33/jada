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

class ExistingModuleImportCheck : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
};

class GlobalSpaceCreation : public ISemanticsPart {
public:
    std::string analyse(
            const std::vector<
                std::shared_ptr<mdl::Module>>& program) override;
private:
    std::pair<bool, std::shared_ptr<node::With>> 
        addImportsPtrs_(std::shared_ptr<mdl::Module> mdl,
        const std::vector<std::shared_ptr<node::IDecl>>& units);

    std::pair<bool,std::shared_ptr<node::Use>>
        addReduceImportPtrs_(std::shared_ptr<mdl::Module> mdl,
        const std::vector<std::shared_ptr<node::IDecl>>& units);
};

} // namespace semantics_part