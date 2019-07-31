! { dg-do run }
program L
   if (and(.TRUE._1, .TRUE._1) .neqv. .true.) STOP 1 ! { dg-warning "GNU Extension" }
   if (or(.TRUE._1, .TRUE._1) .neqv. .true.) STOP 2 ! { dg-warning "GNU Extension" }
   if (xor(.TRUE._1, .TRUE._1) .neqv. .false.) STOP 3 ! { dg-warning "GNU Extension" }
end program L

