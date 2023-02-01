# Danny Lee (dlee692) & Simone Bliss (sbliss5)
## How we made the calulator instance's shared data safe

For our multithreaded network calculator, we used a mutex for our synchronization object. 
We determined the critical sections of code by looking at where simultaneous access would by two different
threads would be problematic. In our program's case, the critcal sections were in the evalExpr function.
In each numbered-argument situation (the expression having 1, 3, or 5 arguments) we locked the mutex
before calling the appropriate operation function and unlocked it after. Though this method might potentially
have more overhead, for the way that we had designed our code specifically, we believed it was 
safer to write it this way than otherwise. 