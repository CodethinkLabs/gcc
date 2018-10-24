! { dg-do run }
!
! Ensure that include line continuations
!

subroutine one()
  include &
  "include_4.inc"
  integer(i4) :: i = 1
  if (i.ne.1) stop 1
end subroutine one

subroutine two()
  include &
! Comment here?!
  &"include_4.inc"
  integer(i4) :: i = 2
  if (i.ne.2) stop 2
end subroutine two

subroutine three()
  inc&
  l&
  &ude &
  &"in&
  &clude_&
! Comment and blank lines?!

! Ridiculous but valid


  &&
  &4.inc"
  integer(i4) :: i = 3
  if (i.ne.3) stop 3
end subroutine three

program test
  call one
  call two
  call three
end program test
