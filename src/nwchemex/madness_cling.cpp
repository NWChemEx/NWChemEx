namespace madness {

class RMI {
    static thread_local bool is_server_thread;
    static bool* _cling_is_server_thread();
};

thread_local bool RMI::is_server_thread = false;

bool* RMI::_cling_is_server_thread() { return &is_server_thread; }

} // namespace madness
