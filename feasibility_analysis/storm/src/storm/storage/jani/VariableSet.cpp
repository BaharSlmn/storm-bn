#include "storm/storage/jani/VariableSet.h"

#include "storm/storage/expressions/Expressions.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace jani {
                
        VariableSet::VariableSet() {
            // Intentionally left empty.
        }
        
        detail::Variables<BooleanVariable> VariableSet::getBooleanVariables() {
            return detail::Variables<BooleanVariable>(booleanVariables.begin(), booleanVariables.end());
        }
        
        detail::ConstVariables<BooleanVariable> VariableSet::getBooleanVariables() const {
            return detail::ConstVariables<BooleanVariable>(booleanVariables.begin(), booleanVariables.end());
        }

        detail::Variables<BoundedIntegerVariable> VariableSet::getBoundedIntegerVariables() {
            return detail::Variables<BoundedIntegerVariable>(boundedIntegerVariables.begin(), boundedIntegerVariables.end());
        }

        detail::ConstVariables<BoundedIntegerVariable> VariableSet::getBoundedIntegerVariables() const {
            return detail::ConstVariables<BoundedIntegerVariable>(boundedIntegerVariables.begin(), boundedIntegerVariables.end());
        }

        detail::Variables<UnboundedIntegerVariable> VariableSet::getUnboundedIntegerVariables() {
            return detail::Variables<UnboundedIntegerVariable>(unboundedIntegerVariables.begin(), unboundedIntegerVariables.end());
        }

        detail::ConstVariables<UnboundedIntegerVariable> VariableSet::getUnboundedIntegerVariables() const {
            return detail::ConstVariables<UnboundedIntegerVariable>(unboundedIntegerVariables.begin(), unboundedIntegerVariables.end());
        }

        detail::Variables<RealVariable> VariableSet::getRealVariables() {
            return detail::Variables<RealVariable>(realVariables.begin(), realVariables.end());
        }
        
        detail::ConstVariables<RealVariable> VariableSet::getRealVariables() const {
            return detail::ConstVariables<RealVariable>(realVariables.begin(), realVariables.end());
        }
        
        detail::Variables<ArrayVariable> VariableSet::getArrayVariables() {
            return detail::Variables<ArrayVariable>(arrayVariables.begin(), arrayVariables.end());
        }
        
        detail::ConstVariables<ArrayVariable> VariableSet::getArrayVariables() const {
            return detail::ConstVariables<ArrayVariable>(arrayVariables.begin(), arrayVariables.end());
        }
        
        detail::Variables<ClockVariable> VariableSet::getClockVariables() {
            return detail::Variables<ClockVariable>(clockVariables.begin(), clockVariables.end());
        }
        
        detail::ConstVariables<ClockVariable> VariableSet::getClockVariables() const {
            return detail::ConstVariables<ClockVariable>(clockVariables.begin(), clockVariables.end());
        }
        
        Variable const& VariableSet::addVariable(Variable const& variable) {
            if (variable.isBooleanVariable()) {
                return addVariable(variable.asBooleanVariable());
            } else if (variable.isBoundedIntegerVariable()) {
                return addVariable(variable.asBoundedIntegerVariable());
            } else if (variable.isUnboundedIntegerVariable()) {
                return addVariable(variable.asUnboundedIntegerVariable());
            } else if (variable.isRealVariable()) {
                return addVariable(variable.asRealVariable());
            } else if (variable.isArrayVariable()) {
                return addVariable(variable.asArrayVariable());
            } else if (variable.isClockVariable()) {
                return addVariable(variable.asClockVariable());
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Cannot add variable of unknown type.");
        }


        BooleanVariable const& VariableSet::addVariable(BooleanVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<BooleanVariable> newVariable = std::make_shared<BooleanVariable>(variable);
            variables.push_back(newVariable);
            booleanVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        BoundedIntegerVariable const& VariableSet::addVariable(BoundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<BoundedIntegerVariable> newVariable = std::make_shared<BoundedIntegerVariable>(variable);
            variables.push_back(newVariable);
            boundedIntegerVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        UnboundedIntegerVariable const& VariableSet::addVariable(UnboundedIntegerVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<UnboundedIntegerVariable> newVariable = std::make_shared<UnboundedIntegerVariable>(variable);
            variables.push_back(newVariable);
            unboundedIntegerVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        RealVariable const& VariableSet::addVariable(RealVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<RealVariable> newVariable = std::make_shared<RealVariable>(variable);
            variables.push_back(newVariable);
            realVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        ArrayVariable const& VariableSet::addVariable(ArrayVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<ArrayVariable> newVariable = std::make_shared<ArrayVariable>(variable);
            variables.push_back(newVariable);
            arrayVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        ClockVariable const& VariableSet::addVariable(ClockVariable const& variable) {
            STORM_LOG_THROW(!this->hasVariable(variable.getName()), storm::exceptions::WrongFormatException, "Cannot add variable with name '" << variable.getName() << "', because a variable with that name already exists.");
            std::shared_ptr<ClockVariable> newVariable = std::make_shared<ClockVariable>(variable);
            variables.push_back(newVariable);
            clockVariables.push_back(newVariable);
            if (variable.isTransient()) {
                transientVariables.push_back(newVariable);
            }
            nameToVariable.emplace(variable.getName(), variable.getExpressionVariable());
            variableToVariable.emplace(variable.getExpressionVariable(), newVariable);
            return *newVariable;
        }
        
        std::vector<std::shared_ptr<ArrayVariable>> VariableSet::dropAllArrayVariables() {
            if (!arrayVariables.empty()) {
                for (auto const& arrVar : arrayVariables) {
                    nameToVariable.erase(arrVar->getName());
                    variableToVariable.erase(arrVar->getExpressionVariable());
                }
                std::vector<std::shared_ptr<Variable>> newVariables;
                for (auto const& v : variables) {
                    if (!v->isArrayVariable()) {
                        newVariables.push_back(v);
                    }
                }
                variables = std::move(newVariables);
                newVariables.clear();
                for (auto const& v : transientVariables) {
                    if (!v->isArrayVariable()) {
                        newVariables.push_back(v);
                    }
                }
                transientVariables = std::move(newVariables);
            }
            
            std::vector<std::shared_ptr<ArrayVariable>> result = std::move(arrayVariables);
            arrayVariables.clear();
            return result;
        }
        
        bool VariableSet::hasVariable(std::string const& name) const {
            return nameToVariable.find(name) != nameToVariable.end();
        }

        bool VariableSet::hasVariable(Variable const& var) const {
            return hasVariable(var.getName());
        }

        Variable const& VariableSet::getVariable(std::string const& name) const {
            auto it = nameToVariable.find(name);
            STORM_LOG_THROW(it != nameToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << name << "'.");
            return getVariable(it->second);
        }

        template <typename VarType>
        void eraseFromVariableVector(std::vector<std::shared_ptr<VarType>>& varVec, storm::expressions::Variable const& variable) {
            for (auto vIt = varVec.begin(); vIt != varVec.end(); ++vIt) {
                if ((*vIt)->getExpressionVariable() == variable) {
                    varVec.erase(vIt);
                    break;
                }
            }
        }
        
        std::shared_ptr<Variable> VariableSet::eraseVariable(storm::expressions::Variable const& variable) {
            auto vToVIt = variableToVariable.find(variable);
            STORM_LOG_THROW(vToVIt != variableToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to erase unknown variable '" << variable.getName() << "'.");
            std::shared_ptr<Variable> janiVar = std::move(vToVIt->second);
            variableToVariable.erase(vToVIt);
            
            nameToVariable.erase(janiVar->getName());
            eraseFromVariableVector(variables, variable);
            if (janiVar->isBooleanVariable()) {
                eraseFromVariableVector(booleanVariables, variable);
            }
            if (janiVar->isBooleanVariable()) {
                eraseFromVariableVector(booleanVariables, variable);
            }
            if (janiVar->isBoundedIntegerVariable()) {
                eraseFromVariableVector(boundedIntegerVariables, variable);
            }
            if (janiVar->isUnboundedIntegerVariable()) {
                eraseFromVariableVector(unboundedIntegerVariables, variable);
            }
            if (janiVar->isRealVariable()) {
                eraseFromVariableVector(realVariables, variable);
            }
            if (janiVar->isArrayVariable()) {
                eraseFromVariableVector(arrayVariables, variable);
            }
            if (janiVar->isClockVariable()) {
                eraseFromVariableVector(clockVariables, variable);
            }
            if (janiVar->isTransient()) {
                eraseFromVariableVector(transientVariables, variable);
            }
            return janiVar;
        }

        typename detail::Variables<Variable>::iterator VariableSet::begin() {
            return detail::Variables<Variable>::make_iterator(variables.begin());
        }

        typename detail::ConstVariables<Variable>::iterator VariableSet::begin() const {
            return detail::ConstVariables<Variable>::make_iterator(variables.begin());
        }
        
        typename detail::Variables<Variable>::iterator VariableSet::end() {
            return detail::Variables<Variable>::make_iterator(variables.end());
        }

        detail::ConstVariables<Variable>::iterator VariableSet::end() const {
            return detail::ConstVariables<Variable>::make_iterator(variables.end());
        }

        Variable const& VariableSet::getVariable(storm::expressions::Variable const& variable) const {
            auto it = variableToVariable.find(variable);
            STORM_LOG_THROW(it != variableToVariable.end(), storm::exceptions::InvalidArgumentException, "Unable to retrieve unknown variable '" << variable.getName() << "'.");
            return *it->second;
        }
        
        bool VariableSet::hasVariable(storm::expressions::Variable const& variable) const {
            return variableToVariable.find(variable) != variableToVariable.end();
        }
        
        bool VariableSet::hasTransientVariable() const {
            for (auto const& variable : variables) {
                if (variable->isTransient()) {
                    return true;
                }
            }
            return false;
        }
        
        bool VariableSet::containsBooleanVariable() const {
            return !booleanVariables.empty();
        }
        
        bool VariableSet::containsBoundedIntegerVariable() const {
            return !boundedIntegerVariables.empty();
        }
        
        bool VariableSet::containsUnboundedIntegerVariables() const {
            return !unboundedIntegerVariables.empty();
        }
        
        bool VariableSet::containsRealVariables() const {
            return !realVariables.empty();
        }
        
        bool VariableSet::containsArrayVariables() const {
            return !arrayVariables.empty();
        }
        
        bool VariableSet::containsClockVariables() const {
            return !clockVariables.empty();
        }
        
        bool VariableSet::containsNonTransientRealVariables() const {
            for (auto const& variable : realVariables) {
                if (!variable->isTransient()) {
                    return true;
                }
            }
            return false;
        }
        
        bool VariableSet::containsNonTransientUnboundedIntegerVariables() const {
            for (auto const& variable : unboundedIntegerVariables) {
                if (!variable->isTransient()) {
                    return true;
                }
            }
            return false;
        }
        
        bool VariableSet::empty() const {
            return !(containsBooleanVariable() || containsBoundedIntegerVariable() || containsUnboundedIntegerVariables() || containsRealVariables() || containsArrayVariables() || containsClockVariables());
        }

        uint64_t VariableSet::getNumberOfVariables() const {
            return variables.size();
        }


        uint64_t VariableSet::getNumberOfNontransientVariables() const {
            return getNumberOfVariables() - getNumberOfTransientVariables();
        }

        uint_fast64_t VariableSet::getNumberOfTransientVariables() const {
            uint_fast64_t result = 0;
            for (auto const& variable : variables) {
                if (variable->isTransient()) {
                    ++result;
                }
            }
            return result;
        }
        
        uint_fast64_t VariableSet::getNumberOfRealTransientVariables() const {
            uint_fast64_t result = 0;
            for (auto const& variable : variables) {
                if (variable->isTransient() && variable->isRealVariable()) {
                    ++result;
                }
            }
            return result;
        }
        
        uint_fast64_t VariableSet::getNumberOfUnboundedIntegerTransientVariables() const {
            uint_fast64_t result = 0;
            for (auto const& variable : variables) {
                if (variable->isTransient() && variable->isUnboundedIntegerVariable()) {
                    ++result;
                }
            }
            return result;
        }
        
        uint_fast64_t VariableSet::getNumberOfNumericalTransientVariables() const {
            uint_fast64_t result = 0;
            for (auto const& variable : transientVariables) {
                if (variable->isRealVariable() || variable->isUnboundedIntegerVariable() || variable->isBoundedIntegerVariable()) {
                    ++result;
                }
            }
            return result;
        }
        
        typename detail::ConstVariables<Variable> VariableSet::getTransientVariables() const {
            return detail::ConstVariables<Variable>(transientVariables.begin(), transientVariables.end());
        }
        
        bool VariableSet::containsVariablesInBoundExpressionsOrInitialValues(std::set<storm::expressions::Variable> const& variables) const {
            for (auto const& booleanVariable : this->getBooleanVariables()) {
                if (booleanVariable.hasInitExpression()) {
                    if (booleanVariable.getInitExpression().containsVariable(variables)) {
                        return true;
                    }
                }
            }
            for (auto const& integerVariable : this->getBoundedIntegerVariables()) {
                if (integerVariable.hasInitExpression()) {
                    if (integerVariable.getInitExpression().containsVariable(variables)) {
                        return true;
                    }
                }
                if (integerVariable.getLowerBound().containsVariable(variables)) {
                    return true;
                }
                if (integerVariable.getUpperBound().containsVariable(variables)) {
                    return true;
                }
            }
            for (auto const& arrayVariable : this->getArrayVariables()) {
                if (arrayVariable.hasInitExpression()) {
                    if (arrayVariable.getInitExpression().containsVariable(variables)) {
                        return true;
                    }
                }
                if (arrayVariable.hasLowerElementTypeBound()) {
                    if (arrayVariable.getLowerElementTypeBound().containsVariable(variables)) {
                        return true;
                    }
                }
                if (arrayVariable.hasUpperElementTypeBound()) {
                    if (arrayVariable.getUpperElementTypeBound().containsVariable(variables)) {
                        return true;
                    }
                }
            }
            for (auto const& clockVariable : this->getClockVariables()) {
                if (clockVariable.hasInitExpression()) {
                    if (clockVariable.getInitExpression().containsVariable(variables)) {
                        return true;
                    }
                }
            }
            return false;
        }
        
        std::map<std::string, std::reference_wrapper<Variable const>> VariableSet::getNameToVariableMap() const {
            std::map<std::string, std::reference_wrapper<Variable const>> result;
            
            for (auto const& variable : variables) {
                result.emplace(variable->getName(), *variable);
            }
            
            return result;
        }
        
        void VariableSet::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            for (auto& variable : variables) {
                variable->substitute(substitution);
            }
        }
        
        void VariableSet::substituteExpressionVariables(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            for (auto& variable : variables) {
                auto varIt = substitution.find(variable->getExpressionVariable());
                if (varIt != substitution.end()) {
                    STORM_LOG_ASSERT(varIt->second.isVariable(), "Expected that variables are only substituted by other variables. However, we substitute " << varIt->first.getName() << " by " << varIt->second << ".");
                    variable->setExpressionVariable(varIt->second.getBaseExpression().asVariableExpression().getVariable());
                }
            }
        }
    }
}
