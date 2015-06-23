! { dg-do run }
! { dg-options "-std=extra-legacy" }

program test
  implicit none
  integer :: x
  integer :: x
  x = 42
  if (X /= 42) stop 1
end program test
