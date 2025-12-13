#include "beacon_compatibility.h"

/* Token Functions */

BOOL beacon_use_token(HANDLE token) {
    /* Probably needs to handle DuplicateTokenEx too */
    SetThreadToken(NULL, token);
    return TRUE;
}

VOID beacon_revert_token(VOID) {
    if (!RevertToSelf()) 
        _inf_coffloader("RevertToSelf Failed!\n");
    
    return;
}

BOOL beacon_is_admin(VOID) {
    BOOL isAdmin             = FALSE;
    PSID administratorsGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    // Créer le SID du groupe Administrators
    if (AllocateAndInitializeSid(
            &ntAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &administratorsGroup)) {
        
        // Vérifier si le token actuel appartient au groupe Administrators
        if (!CheckTokenMembership(NULL, administratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        
        FreeSid(administratorsGroup);
    }

    return isAdmin;
}