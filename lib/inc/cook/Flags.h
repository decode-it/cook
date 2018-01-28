#ifndef HEADER_cook_Flags_h_ALREADY_INCLUDED
#define HEADER_cook_Flags_h_ALREADY_INCLUDED

typedef enum
{
    cook_Overwrite_Default = 0,
    cook_Overwrite_Never,
    cook_Overwrite_IfSame,
    cook_Overwrite_Always,
} cook_Overwrite_t;

typedef enum
{
    cook_Propagation_Default,
    cook_Propagation_Public,
    cook_Propagation_Private,
} cook_Propagation_t;

typedef enum
{
    cook_Flag_None = 0,
    cook_Flag_ForceInclude = 0x01,
} cook_Flag_t;

typedef struct
{
    cook_Overwrite_t overwrite;
    cook_Propagation_t propagation;
    cook_Flag_t additional;

} cook_Flags_T;


#ifdef __cplusplus
extern "C" {
#endif

cook_Flags_T cook_Flag_T_default();

#ifdef __cplusplus
}
#endif



typedef enum
{
    cook_Flags_Overwrite_Never      = 0x01,
    cook_Flags_Overwrite_IfSame     = 0x02,
    cook_Flags_Overwrite_Always     = 0x04,
    cook_Flags_Propagation_Public   = 0x08,
    cook_Flags_Propagation_Private  = 0x10,
    cook_Flags_ForceInclude         = 0x20,

} cook_Flags_t;



#endif
