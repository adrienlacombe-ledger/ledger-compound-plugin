#include "compound_plugin.h"

// One param functions handler
static void handle_one_param_function(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case MINT_AMOUNT:  // mintAmount
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case REDEEM_TOKENS:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case REDEEM_AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case BORROW_AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case REPAY_AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case DELEGATEE:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Repay borrow on behalf handler
static void repay_borrow_on_behalf(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case BORROWER:  // mintAmount
            copy_address(context->dest, msg->parameter, sizeof(context->dest));
            context->next_param = REPAY_AMOUNT;
            break;
        case REPAY_AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Vote cast or Manual vote
static void manual_vote(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case PROPOSAL_ID:  // PROPOSAl_ID
            copy_parameter(context->proposal_id, msg->parameter, sizeof(context->proposal_id));
            context->next_param = SUPPORT;
            break;
        case SUPPORT:
            copy_parameter(context->support, msg->parameter, sizeof(context->support));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Transfer function handler
static void transfer_tokens(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case RECIPIENT:  // mintAmount
            copy_address(context->dest, msg->parameter, sizeof(context->dest));
            context->next_param = AMOUNT;
            break;
        case AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// Liquidate borrow handler
static void liquidate_borrow(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }
    switch (context->next_param) {
        case BORROWER:  // borrower
            copy_address(context->dest, msg->parameter, sizeof(context->dest));
            context->next_param = AMOUNT;
            break;
        case AMOUNT:
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = COLLATERAL;
            break;
        case COLLATERAL:
            copy_address(context->collateral, msg->parameter, sizeof(context->collateral));
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
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case COMPOUND_MINT:
            handle_one_param_function(msg, context);
            break;
        case COMPOUND_REDEEM:
            handle_one_param_function(msg, context);
            break;
        case COMPOUND_REDEEM_UNDERLYING:
            handle_one_param_function(msg, context);
            break;
        case COMPOUND_BORROW:
            handle_one_param_function(msg, context);
            break;
        case COMPOUND_REPAY_BORROW:
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
        case COMPOUND_MANUAL_VOTE:
            manual_vote(msg, context);
            break;
        case COMPOUND_VOTE_DELEGATE:
            handle_one_param_function(msg, context);
            break;
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
