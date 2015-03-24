//=======================================================================
//
//=======================================================================

#ifndef KBD_F_
#define KBD_F_

#include <string>

std::string GetLine ();
char* GetKey ();
int SetEcho (int new_status);
int SetCanonical(int new_status);

#endif  // KBD_F_
