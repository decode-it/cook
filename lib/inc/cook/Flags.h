#ifndef HEADER_cook_Flags_h_ALREADY_INCLUDED
#define HEADER_cook_Flags_h_ALREADY_INCLUDED

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
