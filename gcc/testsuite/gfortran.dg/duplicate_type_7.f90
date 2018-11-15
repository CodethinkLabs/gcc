! { dg-do run }
! { dg-options "-std=legacy -fdec-duplicates" }

program test
  implicit none
  integer :: x
  integer :: x
  x = 42
  if (x /= 42) stop 1
end program test
