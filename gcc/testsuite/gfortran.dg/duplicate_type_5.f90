! { dg-do run }
! { dg-options "-fdec" }

program test
  implicit none
  integer :: x
  integer:: x
  x = 42
  if (x /= 42) stop 1
end program test
