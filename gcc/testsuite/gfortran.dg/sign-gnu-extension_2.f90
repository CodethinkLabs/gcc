! { dg-do run }
! { dg-options "-Wconversion-extra" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>
!
program test
  implicit none
  real(4) :: a4 = 4.0_4
  real(8) :: a8 = 8.0_8
  real(16) :: a16 = 16.0_16
  real(4) :: p4 = 1.0_4
  real(8) :: p8 = -1.0_8
  real(16) :: p16 = 1.0_16
  real(8), parameter :: delta8 = 1.0e-6_8
  real(16), parameter :: delta16 = 1.0e-6_16

  if (sign(a4, p8)-4.0_8.gt.delta8) stop 1    ! { dg-warning "Conversion from" }
  if (kind(sign(a4, p8)).ne.8) stop 2         ! { dg-warning "Conversion from" }
  if (sign(1.0_4, p8)-1.0_8.gt.delta8) stop 3 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_4, p8)).ne.8) stop 4      ! { dg-warning "Conversion from" }
  if (sign(1.0_4, 1.0_8)-1.0_8.gt.delta8) stop 5
  if (kind(sign(1.0_4, 1.0_8)).ne.8) stop 6
  !
  if (sign(a4, p16)-4.0_16.gt.delta16) stop 7    ! { dg-warning "Conversion from" }
  if (kind(sign(a4, p16)).ne.16) stop 8          ! { dg-warning "Conversion from" }
  if (sign(1.0_4, p16)-1.0_16.gt.delta16) stop 9 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_4, p16)).ne.16) stop 10      ! { dg-warning "Conversion from" }
  if (sign(1.0_4, 1.0_16)-1.0_16.gt.delta16) stop 11
  if (kind(sign(1.0_4, 1.0_16)).ne.16) stop 12
  !!
  if (sign(a8, p4)-8.0_8.gt.delta8) stop 13    ! { dg-warning "Conversion from" }
  if (kind(sign(a8, p4)).ne.8) stop 14         ! { dg-warning "Conversion from" }
  if (sign(1.0_8, p4)-1.0_8.gt.delta8) stop 15 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_8, p4)).ne.8) stop 16      ! { dg-warning "Conversion from" }
  if (sign(1.0_8, 1.0_4)-1.0_8.gt.delta8) stop 17
  if (kind(sign(1.0_8, 1.0_4)).ne.8) stop 18
  !
  if (sign(a8, p16)-8.0_16.gt.delta16) stop 19    ! { dg-warning "Conversion from" }
  if (kind(sign(a8, p16)).ne.16) stop 20          ! { dg-warning "Conversion from" }
  if (sign(1.0_8, p16)-1.0_16.gt.delta16) stop 21 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_8, p16)).ne.16) stop 22       ! { dg-warning "Conversion from" }
  if (sign(1.0_8, 1.0_16)-1.0_16.gt.delta16) stop 23
  if (kind(sign(1.0_8, 1.0_16)).ne.16) stop 24
  !!
  if (sign(a16, p4)-16.0_16.gt.delta16) stop 25   ! { dg-warning "Conversion from" }
  if (kind(sign(a16, p4)).ne.16) stop 26          ! { dg-warning "Conversion from" }
  if (sign(1.0_16, p4)-1.0_16.gt.delta16) stop 27 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_16, p4)).ne.16) stop 28       ! { dg-warning "Conversion from" }
  if (sign(1.0_16, 1.0_4)-1.0_16.gt.delta16) stop 29
  if (kind(sign(1.0_16, 1.0_4)).ne.16) stop 30
  !
  if (sign(a16, p8)-16.0_16.gt.delta16) stop 31   ! { dg-warning "Conversion from" }
  if (kind(sign(a16, p8)).ne.16) stop 32          ! { dg-warning "Conversion from" }
  if (sign(1.0_16, p8)-1.0_16.gt.delta16) stop 33 ! { dg-warning "Conversion from" }
  if (kind(sign(1.0_16, p8)).ne.16) stop 34       ! { dg-warning "Conversion from" }
  if (sign(1.0_16, 1.0_8)-1.0_16.gt.delta16) stop 35
  if (kind(sign(1.0_16, 1.0_8)).ne.16) stop 36
end program test


