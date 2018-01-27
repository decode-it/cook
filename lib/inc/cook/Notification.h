#ifndef HEADER_cook_Notification_h_ALREADY_INCLUDED
#define HEADER_cook_Notification_h_ALREADY_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    cook_NotificationType_Message,
    cook_NotificationType_Info,
    cook_NotificationType_Warning,
    cook_NotificationType_Error,
    cook_NotificationType_Fatal,
    cook_NotificationType_Always

} cook_NotificationType_t;

typedef void (*cook_NotificationCallback_t)(void * context, cook_NotificationType_t type, const char * message);


#ifdef __cplusplus
}
#endif

#endif
