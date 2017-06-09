#include <common/rpc_helper.h>
#include <common/log.h>

static const char *type_to_name(int message_type)
{
    switch (message_type) {
    case DBUS_MESSAGE_TYPE_SIGNAL:
        return "signal";
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
        return "method call";
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
        return "method return";
    case DBUS_MESSAGE_TYPE_ERROR:
        return "error";
    default:
        return "(unknown message type)";
    }
}

static void indent(int depth)
{
    while (depth-- > 0)
        log_debug("   ");
}

void print_iter(DBusMessageIter *iter, dbus_bool_t literal, int depth)
{
    do {
        int type = dbus_message_iter_get_arg_type(iter);

        if (type == DBUS_TYPE_INVALID)
            break;

        indent(depth);

        switch (type) {
        case DBUS_TYPE_STRING: {
            char *val;
            dbus_message_iter_get_basic(iter, &val);
            if (!literal)
                log_debug("string \"");
            log_debug("%s", val);
            if (!literal)
                log_debug("\"\n");
            break;
        }

        case DBUS_TYPE_SIGNATURE: {
            char *val;
            dbus_message_iter_get_basic(iter, &val);
            if (!literal)
                log_debug("signature \"");
            log_debug("%s", val);
            if (!literal)
                log_debug("\"\n");
            break;
        }

        case DBUS_TYPE_OBJECT_PATH: {
            char *val;
            dbus_message_iter_get_basic(iter, &val);
            if (!literal)
                log_debug("object path \"");
            log_debug("%s", val);
            if (!literal)
                log_debug("\"\n");
            break;
        }

        case DBUS_TYPE_INT16: {
            dbus_int16_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("int16 %d\n", val);
            break;
        }

        case DBUS_TYPE_UINT16: {
            dbus_uint16_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("uint16 %u\n", val);
            break;
        }

        case DBUS_TYPE_INT32: {
            dbus_int32_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("int32 %d\n", val);
            break;
        }

        case DBUS_TYPE_UINT32: {
            dbus_uint32_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("uint32 %u\n", val);
            break;
        }

        case DBUS_TYPE_INT64: {
            dbus_int64_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("int64 %ld\n", val);
            break;
        }

        case DBUS_TYPE_UINT64: {
            dbus_uint64_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("uint64 %lu\n", val);
            break;
        }

        case DBUS_TYPE_DOUBLE: {
            double val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("double %g\n", val);
            break;
        }

        case DBUS_TYPE_BYTE: {
            unsigned char val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("byte %d\n", val);
            break;
        }

        case DBUS_TYPE_BOOLEAN: {
            dbus_bool_t val;
            dbus_message_iter_get_basic(iter, &val);
            log_debug("boolean %s\n", val ? "true" : "false");
            break;
        }

        case DBUS_TYPE_VARIANT: {
            DBusMessageIter subiter;

            dbus_message_iter_recurse(iter, &subiter);

            log_debug("variant ");
            print_iter(&subiter, literal, depth + 1);
            break;
        }

        case DBUS_TYPE_ARRAY: {
            int current_type;
            DBusMessageIter subiter;

            dbus_message_iter_recurse(iter, &subiter);

            log_debug("array [\n");
            while ((current_type = dbus_message_iter_get_arg_type(&subiter)) != DBUS_TYPE_INVALID) {
                print_iter(&subiter, literal, depth + 1);
                dbus_message_iter_next(&subiter);
                if (dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_INVALID)
                    log_debug(",");
            }
            indent(depth);
            log_debug("]\n");
            break;
        }

        case DBUS_TYPE_DICT_ENTRY: {
            DBusMessageIter subiter;

            dbus_message_iter_recurse(iter, &subiter);

            log_debug("dict entry(\n");
            print_iter(&subiter, literal, depth + 1);
            dbus_message_iter_next(&subiter);
            print_iter(&subiter, literal, depth + 1);
            indent(depth);
            log_debug(")\n");
            break;
        }

        case DBUS_TYPE_STRUCT: {
            int current_type;
            DBusMessageIter subiter;

            dbus_message_iter_recurse(iter, &subiter);

            log_debug("struct {\n");
            while ((current_type = dbus_message_iter_get_arg_type(&subiter)) != DBUS_TYPE_INVALID) {
                print_iter(&subiter, literal, depth + 1);
                dbus_message_iter_next(&subiter);
                if (dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_INVALID)
                    log_debug(",");
            }
            indent(depth);
            log_debug("}\n");
            break;
        }

        default:
            log_debug(" (dbus-monitor too dumb to decipher arg type '%c')\n", type);
            break;
        }
    } while (dbus_message_iter_next(iter));
}

void print_rpc_msg(DBusMessage *message, dbus_bool_t literal)
{
    DBusMessageIter iter;
    const char *sender;
    const char *destination;
    int message_type;

    message_type = dbus_message_get_type(message);
    sender = dbus_message_get_sender(message);
    destination = dbus_message_get_destination(message);

    if (!literal) {
        log_debug("%s sender=%s -> dest=%s", type_to_name(message_type),
               sender ? sender : "(null sender)", destination ? destination : "(null destination)");

        switch (message_type) {
        case DBUS_MESSAGE_TYPE_METHOD_CALL:
        case DBUS_MESSAGE_TYPE_SIGNAL:
            log_debug(" path=%s; interface=%s; member=%s\n", dbus_message_get_path(message),
                   dbus_message_get_interface(message), dbus_message_get_member(message));
            break;

        case DBUS_MESSAGE_TYPE_METHOD_RETURN:
            log_debug("\n");
            break;

        case DBUS_MESSAGE_TYPE_ERROR:
            log_debug(" error_name=%s\n", dbus_message_get_error_name(message));
            break;

        default:
            log_debug("\n");
            break;
        }
    }

    dbus_message_iter_init(message, &iter);
    print_iter(&iter, literal, 1);
    // fflush(stdout);
}
