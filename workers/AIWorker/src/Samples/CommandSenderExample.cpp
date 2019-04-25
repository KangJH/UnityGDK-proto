#include "Samples/SpatialOSUtilitiesExamples.h"

#include "improbable/worker.h"

#include "Samples/IScheduler.h"
#include "Samples/SampleRetryNotifier.h"
#include "Samples/SampleCommandSenders.h"

#include "CommandSender/RetryStrategyConfig.h"

//#include "Samples/CommandsTest.h"

/*****************************************************************************************************************************************/
/*                                                                                                                                       */
/* Sample CommandSender usage:                                                                                                           */
/*                                                                                                                                       */
/* worker::EntityID targetEntity;                                                                                                        */
/* CommandType::Request requestPayload;                                                                                                  */
/* int timeout_ms;                                                                                                                       */
/*                                                                                                                                       */
/* //Discard command response if we don't care:                                                                                          */
/* CommandSenders::FireAndForget->Send<CommandType>(targetEntity, requestPayload, timeout_ms);                                           */
/*                                                                                                                                       */
/*                                                                                                                                       */
/* //Bind to command response:                                                                                                           */
/* CommandResponse<CommandType> response = CommandSenders::FireAndForget->Send<ACommandName>(targetEntity, requestPayload, timeout_ms);  */
/* response.OnSuccess([](const CommandType::Response&) { //do something });                                                              */
/* response.OnFailure([](worker::StatusCode, const std::string&) { //do something });                                                    */
/*                                                                                                                                       */
/*****************************************************************************************************************************************/

void SpatialOSSamples::SetupDefaultCommandSenders(worker::Connection& connection, worker::Dispatcher& dispatcher, const std::shared_ptr<SpatialOSSamples::IScheduler>& scheduler)
{
    /*************************************************************************************************************/
    /* Setup a CommandSender with advised retry strategies                                                      */
    /* ref: https://docs.improbable.io/reference/13.1/csharpsdk/api-reference#improbable-worker-statuscode-enum */
    /************************************************************************************************************/
    std::unordered_map<worker::StatusCode, SpatialOS::RetryStrategy, worker::StatusCodeHash> defaultMapping{
        { worker::StatusCode::kApplicationError, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kPermissionDenied, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kInternalError, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kAuthorityLost, SpatialOS::RetryStrategy::Immediate },
        { worker::StatusCode::kNotFound, SpatialOS::RetryStrategy::Backoff },
        { worker::StatusCode::kTimeout, SpatialOS::RetryStrategy::Backoff } };

    SpatialOS::RetryStrategyConfig defaultRetryConfig{ defaultMapping, 5 };
    std::unique_ptr<SpatialOS::IRetryNotifier> defaultStrategy = std::make_unique<SpatialOSSamples::SampleRetryNotifier>(defaultRetryConfig, scheduler);
    SpatialOSSamples::SampleCommandSenders::Default = std::make_unique<SpatialOS::CommandSender>(std::move(defaultStrategy), connection, dispatcher);


    /***************************************************************************/
    /* Setup a CommandSender for Low Priority commands (never retry)           */
    /***************************************************************************/
    std::unordered_map<worker::StatusCode, SpatialOS::RetryStrategy, worker::StatusCodeHash> lowPriorityMapping{
        { worker::StatusCode::kApplicationError, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kPermissionDenied, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kInternalError, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kAuthorityLost, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kNotFound, SpatialOS::RetryStrategy::Never },
        { worker::StatusCode::kTimeout, SpatialOS::RetryStrategy::Never } };

    SpatialOS::RetryStrategyConfig lowPriorityConfig{ lowPriorityMapping, 0 };
    std::unique_ptr<SpatialOS::IRetryNotifier> lowPriorityStrategy = std::make_unique<SpatialOSSamples::SampleRetryNotifier>(lowPriorityConfig, scheduler);
    SpatialOSSamples::SampleCommandSenders::LowPriority = std::make_unique<SpatialOS::CommandSender>(std::move(lowPriorityStrategy), connection, dispatcher);

    //Run Command retry tests
    //SpatialOSSamples::Tests::CommandRetriesTest(0);
}
