! { dg-do compile }
!
! Ensure that we handle the pathname on a separate line than
! the include directivbe
!

subroutine one()
  include
   "include_4.inc"
  integer(i4) :: i
end subroutine one
