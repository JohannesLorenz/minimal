* **Q**: How realtime safe is minimal?
* **A**: There are two modes. In single-threaded mode, it is likely to be realtime
   safe. This means: No syscalls in jack's process callback. For the
   multithreaded mode, we are not strictly realtime safe: We need to use
   semaphores, which use syscalls. However, these context switches are usually
   considered safe, since the threads have a high priority (SCHED_FIFO). The
   `JACK 2' library uses the same principle.

* **Q**: Why do you offer multiple engines, but only jack is allowed for connecting
   to zynaddsubfx?
* **A**: For multiple reasons:
  * For the engines, you might need different choices, depending on what
    you like to do with the output - forwarding, connecting, ...
  * You could interpret data in different ways: a sine wave can be a sound
    or a math function or ...
  * It was easier to code
