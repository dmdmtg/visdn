/* NCR's libsocket.so has this undefined symbol which causes
 * our "cc -o visdn" to fail.
 * So, define the variable (or is it a function?) here to keep the
 * compiler happy.
*/
int	_abi_innetgr;
