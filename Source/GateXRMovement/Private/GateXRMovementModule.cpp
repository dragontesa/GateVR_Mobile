// #include "Modules/ModuleManager.h" 
#include "GateXRMovementModule.h"
#include "GateXRMovementLog.h"

#define LOCTEXT_NAMESPACE "GateXRMovement"
DEFINE_LOG_CATEGORY(LogGateXRMovement);


IMPLEMENT_MODULE(FGateXRMovementModule, GateXRMovement);

void FGateXRMovementModule::StartupModule()
{

}

void FGateXRMovementModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE