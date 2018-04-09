!{ dg-do compile }
!{ dg-options "-Wconversion-extra -fdec -fno-dec-promotion" }
!
! integer types of a smaller kind than expected should be
! accepted by type specific intrinsic functions
!
      program test_small_type_promtion
        implicit none
        character(50) :: buffer
        integer(1) :: a1 = 100_1
        integer(2) :: a2 = 20_2
        integer(4) :: a4 = 40_4
        integer(8) :: a8 = 80_8
        integer(1) :: b1 = 7_1
        integer(2) :: b2 = 8_2
        integer(4) :: b4 = 9_4
        integer(8) :: b8 = 11_8
        real(4) :: r4 = 4.0_4
        real(8) :: r8 = 8.0_8
        real(16) :: r16 = 16.0_16
        real(4) :: pr4 = 1.0_4
        real(8) :: pr8 = -1.0_8
        real(16) :: pr16 = 1.0_16
        ! Mix kinds
        if (dim(a1, b2).ne.92_2) stop 1    ! { dg-warning "Conversion from" }
        if (kind(dim(a1, b2)).ne.2) stop 2 ! { dg-warning "Conversion from" }
        if (dim(10_1, b2).ne.2_2) stop 3
        if (kind(dim(10_1, b2)).ne.2) stop 4
        if (dim(10_1, 1_2).ne.9_2) stop 5
        if (kind(dim(10_1, 1_2)).ne.2) stop 6
        !
        if (dim(a1, b4).ne.91_4) stop 7    ! { dg-warning "Conversion from" }
        if (kind(dim(a1, b4)).ne.4) stop 8 ! { dg-warning "Conversion from" }
        if (dim(10_1, b4).ne.1_4) stop 9
        if (kind(dim(10_1, b4)).ne.4) stop 10
        if (dim(10_1, 1_4).ne.9_4) stop 11
        if (kind(dim(10_1, 1_4)).ne.4) stop 12
        !
        if (dim(a1, b8).ne.89_8) stop 13    ! { dg-warning "Conversion from" }
        if (kind(dim(a1, b8)).ne.8) stop 14 ! { dg-warning "Conversion from" }
        if (dim(10_1, b8).ne.0_8) stop 15
        if (kind(dim(1_1, b8)).ne.8) stop 16
        if (dim(10_1, 1_8).ne.9_8) stop 17
        if (kind(dim(10_1, 1_8)).ne.8) stop 18
        !!
        if (dim(a2, b1).ne.13_2) stop 19      ! { dg-warning "Conversion from" }
        if (kind(dim(a2, b1)).ne.2) stop 20   ! { dg-warning "Conversion from" }
        if (dim(10_2, b1).ne.3_2) stop 21     ! { dg-warning "Conversion from" }
        if (kind(dim(10_2, b1)).ne.2) stop 22 ! { dg-warning "Conversion from" }
        if (dim(10_2, 1_1).ne.9_2) stop 23
        if (kind(dim(10_2, 1_1)).ne.2) stop 24
        !
        if (dim(a2, b4).ne.11_4) stop 25     ! { dg-warning "Conversion from" }
        if (kind(dim(a2, b4)).ne.4) stop 26  ! { dg-warning "Conversion from" }
        if (dim(1_2, b4).ne.0_4) stop 27
        if (kind(dim(1_2, b4)).ne.4) stop 28
        if (dim(1_2, 1_4).ne.0_4) stop 29
        if (kind(dim(1_2, 1_4)).ne.4) stop 30
        !
        if (dim(a2, b8).ne.9_8) stop 31     ! { dg-warning "Conversion from" }
        if (kind(dim(a2, b8)).ne.8) stop 32 ! { dg-warning "Conversion from" }
        if (dim(1_2, b8).ne.0_8) stop 33
        if (kind(dim(1_2, b8)).ne.8) stop 34
        if (dim(10_2, 9_8).ne.1_8) stop 35
        if (kind(dim(1_2, 1_8)).ne.8) stop 36
        !!
        if (dim(a4, b1).ne.33_4) stop 37     ! { dg-warning "Conversion from" }
        if (kind(dim(a4, b1)).ne.4) stop 38  ! { dg-warning "Conversion from" }
        if (dim(1_4, b1).ne.0_4) stop 39     ! { dg-warning "Conversion from" }
        if (kind(dim(1_4, b1)).ne.4) stop 40 ! { dg-warning "Conversion from" }
        if (dim(1_4, 1_1).ne.0_4) stop 41
        if (kind(dim(1_4, 1_1)).ne.4) stop 42
        !
        if (dim(a4, b2).ne.32_4) stop 43     ! { dg-warning "Conversion from" }
        if (kind(dim(a4, b2)).ne.4) stop 44  ! { dg-warning "Conversion from" }
        if (dim(1_4, b2).ne.0_4) stop 45     ! { dg-warning "Conversion from" }
        if (kind(dim(1_4, b2)).ne.4) stop 46 ! { dg-warning "Conversion from" }
        if (dim(1_4, 1_2).ne.0_4) stop 47
        if (kind(dim(1_4, 1_2)).ne.4) stop 48
        !
        if (dim(a4, b8).ne.29_8) stop 49     ! { dg-warning "Conversion from" }
        if (kind(dim(a4, b8)).ne.8) stop 50  ! { dg-warning "Conversion from" }
        if (dim(1_4, b8).ne.0_8) stop 51
        if (kind(dim(1_4, b8)).ne.8) stop 52
        if (dim(1_4, 1_8).ne.0_8) stop 53
        if (kind(dim(1_4, 1_8)).ne.8) stop 54
        !!
        if (dim(a8, b1).ne.73_8) stop 55     ! { dg-warning "Conversion from" }
        if (kind(dim(a8, b1)).ne.8) stop 56  ! { dg-warning "Conversion from" }
        if (dim(1_8, b1).ne.0_8) stop 57     ! { dg-warning "Conversion from" }
        if (kind(dim(1_8, b1)).ne.8) stop 58 ! { dg-warning "Conversion from" }
        if (dim(1_8, 1_1).ne.0_8) stop 59
        if (kind(dim(1_8, 1_1)).ne.8) stop 60
        !
        if (dim(a8, b2).ne.72_4) stop 61     ! { dg-warning "Conversion from" }
        if (kind(dim(a8, b2)).ne.8) stop 62  ! { dg-warning "Conversion from" }
        if (dim(1_8, b2).ne.0_8) stop 63     ! { dg-warning "Conversion from" }
        if (kind(dim(1_8, b2)).ne.8) stop 64 ! { dg-warning "Conversion from" }
        if (dim(1_8, 1_2).ne.0_8) stop 65
        if (kind(dim(1_8, 1_2)).ne.8) stop 66
        !
        if (dim(a8, b4).ne.71_8) stop 67     ! { dg-warning "Conversion from" }
        if (kind(dim(a8, b4)).ne.8) stop 68  ! { dg-warning "Conversion from" }
        if (dim(1_8, b4).ne.0_8) stop 69     ! { dg-warning "Conversion from" }
        if (kind(dim(1_8, b4)).ne.8) stop 70 ! { dg-warning "Conversion from" }
        if (dim(1_8, 1_4).ne.0_8) stop 71
        if (kind(dim(1_8, 1_4)).ne.8) stop 72
        !
        ! Mix types - check result is a real
        !
        if (dim(r4, b1).ne.0.0_4) stop 73   ! { dg-error "'x' and 'y' arguments" }
        if (kind(dim(r4, b1)).ne.4) stop 74 ! { dg-error "'x' and 'y' arguments" }
        write(buffer,*) dim(r4, b1)         ! { dg-error "'x' and 'y' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."0.0") stop 75
        if (dim(a1, pr4).ne.99.0_4) stop 76  ! { dg-error "'x' and 'y' arguments" }
        if (kind(dim(a1, pr4)).ne.4) stop 77 ! { dg-error "'x' and 'y' arguments" }
        write(buffer,*) dim(a1, pr4)         ! { dg-error "'x' and 'y' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:4).ne."99.0") stop 78
        !
        if (dim(r8, b2).ne.0.0_8) stop 79   ! { dg-error "'x' and 'y' arguments" }
        if (kind(dim(r8, b2)).ne.8) stop 80 ! { dg-error "'x' and 'y' arguments" }
        write(buffer,*) dim(r8, b2)         ! { dg-error "'x' and 'y' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."0.0") stop 81
        if (dim(a2, pr8).ne.21.0_8) stop 82  ! { dg-error "'x' and 'y' arguments" }
        if (kind(dim(a2, pr8)).ne.8) stop 83 ! { dg-error "'x' and 'y' arguments" }
        write(buffer,*) dim(a2, pr8)         ! { dg-error "'x' and 'y' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:4).ne."21.0") stop 84
      end program
