//
// Created by antonia blanco on 13-02-24.
//

#include "MyExceptions.h"

ObjectiveFunctionNotExecuted::ObjectiveFunctionNotExecuted(const char* message) : message(message) {}
const char* ObjectiveFunctionNotExecuted::what() const noexcept {
    return message;
}

NotImplementedError::NotImplementedError(const char* method_name) : message("The method") {
    message += method_name;
    message += " has not been implemented yet.";}
const char* NotImplementedError::what() const noexcept {
    return message.c_str();
}

ParametersNotSetException::ParametersNotSetException(const char* message) : message(message) {}
const char* ParametersNotSetException::what() const noexcept {
    return message;
}

NoCutsFound::NoCutsFound(const char* message) : message(message) {}
const char* NoCutsFound::what() const noexcept {
    return message;
}

NoCutStrengthening::NoCutStrengthening(const char *message) : message(message) {}
const char* NoCutStrengthening::what() const noexcept {
    return message;
}

MaxFlowNotExecuted::MaxFlowNotExecuted(const char *message) : message(message) {}
const char* MaxFlowNotExecuted::what() const noexcept {
    return message;
}

NoGraphFound::NoGraphFound(const char *message) : message(message) {}
const char* NoGraphFound::what() const noexcept {
    return message;
}

GraphAlreadyExists::GraphAlreadyExists(const char *message) : message(message) {}
const char* GraphAlreadyExists::what() const noexcept {
    return message;
}