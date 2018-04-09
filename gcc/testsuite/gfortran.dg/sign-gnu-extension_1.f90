! { dg-do run }
! { dg-options "-Wconversion-extra" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!
program test
  implicit none
  integer(1) :: a1 = 1_1
  integer(2) :: a2 = 2_2
  integer(4) :: a4 = 4_4
  integer(8) :: a8 = 8_8
  integer(1) :: p1 = 1_1
  integer(2) :: p2 = 1_2
  integer(4) :: p4 = -1_4
  integer(8) :: p8 = -1_8

  if (sign(a1, p2).ne.1_2) stop 1     ! { dg-warning "Conversion from" }
  if (kind(sign(a1, p2)).ne.2) stop 2 ! { dg-warning "Conversion from" }
  if (sign(1_1, p2).ne.1_2) stop 3
  if (kind(sign(1_1, p2)).ne.2) stop 4
  if (sign(1_1, 1_2).ne.1_2) stop 5
  if (kind(sign(1_1, 1_2)).ne.2) stop 6
  !
  if (sign(a1, p4).ne.-1_4) stop 7    ! { dg-warning "Conversion from" }
  if (kind(sign(a1, p4)).ne.4) stop 8 ! { dg-warning "Conversion from" }
  if (sign(1_1, p4).ne.-1_4) stop 9
  if (kind(sign(1_1, p4)).ne.4) stop 10
  if (sign(1_1, 1_4).ne.1_4) stop 11
  if (kind(sign(1_1, 1_4)).ne.4) stop 12
  !
  if (sign(a1, p8).ne.-1_8) stop 13    ! { dg-warning "Conversion from" }
  if (kind(sign(a1, p8)).ne.8) stop 14 ! { dg-warning "Conversion from" }
  if (sign(1_1, p8).ne.-1_8) stop 15
  if (kind(sign(1_1, p8)).ne.8) stop 16
  if (sign(1_1, 1_8).ne.1_8) stop 17
  if (kind(sign(1_1, 1_8)).ne.8) stop 18
  !!
  if (sign(a2, p1).ne.2_2) stop 19      ! { dg-warning "Conversion from" }
  if (kind(sign(a2, p1)).ne.2) stop 20  ! { dg-warning "Conversion from" }
  if (sign(1_2, p1).ne.1_2) stop 21     ! { dg-warning "Conversion from" }
  if (kind(sign(1_2, p1)).ne.2) stop 22 ! { dg-warning "Conversion from" }
  if (sign(1_2, 1_1).ne.1_2) stop 23
  if (kind(sign(1_2, 1_1)).ne.2) stop 24
  !
  if (sign(a2, p4).ne.-2_4) stop 25     ! { dg-warning "Conversion from" }
  if (kind(sign(a2, p4)).ne.4) stop 26 ! { dg-warning "Conversion from" }
  if (sign(1_2, p4).ne.-1_4) stop 27
  if (kind(sign(1_2, p4)).ne.4) stop 28
  if (sign(1_2, 1_4).ne.1_4) stop 29
  if (kind(sign(1_2, 1_4)).ne.4) stop 30
  !
  if (sign(a2, p8).ne.-2_8) stop 31     ! { dg-warning "Conversion from" }
  if (kind(sign(a2, p8)).ne.8) stop 32 ! { dg-warning "Conversion from" }
  if (sign(1_2, p8).ne.-1_8) stop 33
  if (kind(sign(1_2, p8)).ne.8) stop 34
  if (sign(1_2, 1_8).ne.1_8) stop 35
  if (kind(sign(1_2, 1_8)).ne.8) stop 36
  !!
  if (sign(a4, p1).ne.4_4) stop 37      ! { dg-warning "Conversion from" }
  if (kind(sign(a4, p1)).ne.4) stop 38  ! { dg-warning "Conversion from" }
  if (sign(1_4, p1).ne.1_4) stop 39     ! { dg-warning "Conversion from" }
  if (kind(sign(1_4, p1)).ne.4) stop 40 ! { dg-warning "Conversion from" }
  if (sign(1_4, 1_1).ne.1_4) stop 41
  if (kind(sign(1_4, 1_1)).ne.4) stop 42
  !
  if (sign(a4, p2).ne.4_4) stop 43      ! { dg-warning "Conversion from" }
  if (kind(sign(a4, p2)).ne.4) stop 44  ! { dg-warning "Conversion from" }
  if (sign(1_4, p2).ne.1_4) stop 45     ! { dg-warning "Conversion from" }
  if (kind(sign(1_4, p2)).ne.4) stop 46 ! { dg-warning "Conversion from" }
  if (sign(1_4, 1_2).ne.1_4) stop 47
  if (kind(sign(1_4, 1_2)).ne.4) stop 48
  !
  if (sign(a4, p8).ne.-4_8) stop 49     ! { dg-warning "Conversion from" }
  if (kind(sign(a4, p8)).ne.8) stop 50 ! { dg-warning "Conversion from" }
  if (sign(1_4, p8).ne.-1_8) stop 51
  if (kind(sign(1_4, p8)).ne.8) stop 52
  if (sign(1_4, 1_8).ne.1_8) stop 53
  if (kind(sign(1_4, 1_8)).ne.8) stop 54
  !!
  if (sign(a8, p1).ne.8_8) stop 55      ! { dg-warning "Conversion from" }
  if (kind(sign(a8, p1)).ne.8) stop 56  ! { dg-warning "Conversion from" }
  if (sign(1_8, p1).ne.1_8) stop 57     ! { dg-warning "Conversion from" }
  if (kind(sign(1_8, p1)).ne.8) stop 58 ! { dg-warning "Conversion from" }
  if (sign(1_8, 1_1).ne.1_8) stop 59
  if (kind(sign(1_8, 1_1)).ne.8) stop 60
  !
  if (sign(a8, p2).ne.8_4) stop 61      ! { dg-warning "Conversion from" }
  if (kind(sign(a8, p2)).ne.8) stop 62  ! { dg-warning "Conversion from" }
  if (sign(1_8, p2).ne.1_8) stop 63     ! { dg-warning "Conversion from" }
  if (kind(sign(1_8, p2)).ne.8) stop 64 ! { dg-warning "Conversion from" }
  if (sign(1_8, 1_2).ne.1_8) stop 65
  if (kind(sign(1_8, 1_2)).ne.8) stop 66
  !
  if (sign(a8, p4).ne.-8_8) stop 67     ! { dg-warning "Conversion from" }
  if (kind(sign(a8, p4)).ne.8) stop 68  ! { dg-warning "Conversion from" }
  if (sign(1_8, p4).ne.-1_8) stop 69    ! { dg-warning "Conversion from" }
  if (kind(sign(1_8, p4)).ne.8) stop 70 ! { dg-warning "Conversion from" }
  if (sign(1_8, 1_4).ne.1_8) stop 71
  if (kind(sign(1_8, 1_4)).ne.8) stop 72

end program test


