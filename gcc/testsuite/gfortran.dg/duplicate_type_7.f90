! { dg-do run }
! { dg-options "-fdec-duplicates" }

program test
  implicit none
  integer :: x
  integer :: x! { dg-warning "Legacy Extension" }
  x = 42
  if (x /= 42) stop 1
end program test
