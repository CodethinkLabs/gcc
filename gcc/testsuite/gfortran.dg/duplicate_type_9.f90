! { dg-do compile }
! { dg-options "-fdec-duplicates -fno-dec-duplicates" }

integer function foo ()
  implicit none
  integer :: x
  integer :: x ! { dg-error "basic type of" }
  x = 42
end function foo
