! { dg-do  run }
! { dg-shouldfail "Program aborted." }
program main
  call abort ! { dg-warning "GNU Extension" }
end program main
