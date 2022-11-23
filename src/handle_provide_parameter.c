#include "compound_plugin.h"

// One param functions handler
void handle_one_param_function(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case MINT_AMOUNT:
        case REDEEM_TOKENS:
        case REDEEM_AMOUNT:
        case BORROW_AMOUNT:
        case REPAY_AMOUNT:
        case CETH_AMOUNT:
            copy_parameter(context->amount, sizeof(context->amount), msg->parameter);
            context->next_param = UNEXPECTED_PARAMETER;
            context->go_to_offset = true;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Repay borrow on behalf handler
void repay_borrow_on_behalf(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case BORROWER:  // mintAmount
            copy_parameter(context->dest,
                           sizeof(context->dest),
                           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH]);
            context->next_param = REPAY_AMOUNT;
            break;
        case REPAY_AMOUNT:
            copy_parameter(context->amount, sizeof(context->amount), msg->parameter);
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Transfer function handler
void transfer_tokens(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case RECIPIENT:  // mintAmount
            copy_parameter(context->dest,
                           sizeof(context->dest),
                           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH]);
            context->next_param = AMOUNT;
            break;
        case AMOUNT:
            copy_parameter(context->amount, sizeof(context->amount), msg->parameter);
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Liquidate borrow handler
void liquidate_borrow(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case BORROWER:  // borrower
            copy_parameter(context->dest,
                           sizeof(context->dest),
                           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH]);
            context->next_param = AMOUNT;
            break;
        case AMOUNT:
            copy_parameter(context->amount, sizeof(context->amount), msg->parameter);
            context->next_param = COLLATERAL;
            break;
        case COLLATERAL:
            copy_parameter(context->collateral,
                           sizeof(context->dest),
                           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH]);
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    msg->result = ETH_PLUGIN_RESULT_OK;

    if ((context->offset) && msg->parameterOffset != context->checkpoint + context->offset) {
        PRINTF("offset: %d, checkpoint: %d, parameterOffset: %d\n",
               context->offset,
               context->checkpoint,
               msg->parameterOffset);
        return;
    }

    context->offset = 0;  // Reset offset

    // if (context->selectorIndex != CETH_MINT) {
    //     switch (msg->parameterOffset) {
    //         case 4:
    //             memmove(context->amount, msg->parameter, 32);
    //             msg->result = ETH_PLUGIN_RESULT_OK;
    //             break;
    //         default:
    //             PRINTF("Unhandled parameter offset\n");
    //             msg->result = ETH_PLUGIN_RESULT_ERROR;
    //             break;
    //     }
    // } else {
    //     PRINTF("CETH contract expects no parameters\n");
    //     msg->result = ETH_PLUGIN_RESULT_ERROR;
    // }
    switch (context->selectorIndex) {
        case COMPOUND_MINT:
        case COMPOUND_REDEEM:
        case COMPOUND_REDEEM_UNDERLYING:
        case COMPOUND_BORROW:
        case COMPOUND_REPAY_BORROW:
        case CETH_MINT:
            handle_one_param_function(msg, context);
            break;
        case COMPOUND_REPAY_BORROW_ON_BEHALF:
            repay_borrow_on_behalf(msg, context);
            break;
        case COMPOUND_TRANSFER:
            transfer_tokens(msg, context);
            break;
        case COMPOUND_LIQUIDATE_BORROW:
            liquidate_borrow(msg, context);
            break;
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
