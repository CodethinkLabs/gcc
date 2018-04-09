!{ dg-do run }
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
        ! Mix kinds
        if (modulo(a1, b2).ne.4_2) stop 1     ! { dg-warning "Conversion from" }
        if (kind(modulo(a1, b2)).ne.2) stop 2 ! { dg-warning "Conversion from" }
        if (modulo(10_1, b2).ne.2_2) stop 3
        if (kind(modulo(10_1, b2)).ne.2) stop 4
        if (modulo(10_1, 7_2).ne.3_2) stop 5
        if (kind(modulo(10_1, 7_2)).ne.2) stop 6
        !
        if (modulo(a1, b4).ne.1_4) stop 7     ! { dg-warning "Conversion from" }
        if (kind(modulo(a1, b4)).ne.4) stop 8 ! { dg-warning "Conversion from" }
        if (modulo(10_1, b4).ne.1_4) stop 9
        if (kind(modulo(10_1, b4)).ne.4) stop 10
        if (modulo(10_1, 8_4).ne.2_4) stop 11
        if (kind(modulo(10_1, 6_4)).ne.4) stop 12
        !
        if (modulo(a1, b8).ne.1_8) stop 13     ! { dg-warning "Conversion from" }
        if (kind(modulo(a1, b8)).ne.8) stop 14 ! { dg-warning "Conversion from" }
        if (modulo(10_1, b8).ne.10_8) stop 15
        if (kind(modulo(1_1, b8)).ne.8) stop 16
        if (modulo(10_1, 4_8).ne.2_8) stop 17
        if (kind(modulo(10_1, 4_8)).ne.8) stop 18
        !!
        if (modulo(a2, b1).ne.6_2) stop 19       ! { dg-warning "Conversion from" }
        if (kind(modulo(a2, b1)).ne.2) stop 20   ! { dg-warning "Conversion from" }
        if (modulo(10_2, b1).ne.3_2) stop 21     ! { dg-warning "Conversion from" }
        if (kind(modulo(10_2, b1)).ne.2) stop 22 ! { dg-warning "Conversion from" }
        if (modulo(10_2, 6_1).ne.4_2) stop 23
        if (kind(modulo(10_2, 6_1)).ne.2) stop 24
        !
        if (modulo(a2, b4).ne.2_4) stop 25      ! { dg-warning "Conversion from" }
        if (kind(modulo(a2, b4)).ne.4) stop 26  ! { dg-warning "Conversion from" }
        if (modulo(1_2, b4).ne.1_4) stop 27
        if (kind(modulo(1_2, b4)).ne.4) stop 28
        if (modulo(8_2, 3_4).ne.2_4) stop 29
        if (kind(modulo(8_2, 7_4)).ne.4) stop 30
        !
        if (modulo(a2, b8).ne.9_8) stop 31     ! { dg-warning "Conversion from" }
        if (kind(modulo(a2, b8)).ne.8) stop 32 ! { dg-warning "Conversion from" }
        if (modulo(10_2, b8).ne.10_8) stop 33
        if (kind(modulo(10_2, b8)).ne.8) stop 34
        if (modulo(10_2, 9_8).ne.1_8) stop 35
        if (kind(modulo(10_2, 10_8)).ne.8) stop 36
        !!
        if (modulo(a4, b1).ne.5_4) stop 37       ! { dg-warning "Conversion from" }
        if (kind(modulo(a4, b1)).ne.4) stop 38   ! { dg-warning "Conversion from" }
        if (modulo(10_4, b1).ne.3_4) stop 39     ! { dg-warning "Conversion from" }
        if (kind(modulo(10_4, b1)).ne.4) stop 40 ! { dg-warning "Conversion from" }
        if (modulo(10_4, 5_1).ne.0_4) stop 41
        if (kind(modulo(10_4, 7_1)).ne.4) stop 42
        !
        if (modulo(a4, b2).ne.0_4) stop 43       ! { dg-warning "Conversion from" }
        if (kind(modulo(a4, b2)).ne.4) stop 44   ! { dg-warning "Conversion from" }
        if (modulo(10_4, b2).ne.2_4) stop 45     ! { dg-warning "Conversion from" }
        if (kind(modulo(10_4, b2)).ne.4) stop 46 ! { dg-warning "Conversion from" }
        if (modulo(10_4, 5_2).ne.0_4) stop 47
        if (kind(modulo(10_4, 4_2)).ne.4) stop 48
        !
        if (modulo(a4, b8).ne.7_8) stop 49     ! { dg-warning "Conversion from" }
        if (kind(modulo(a4, b8)).ne.8) stop 50 ! { dg-warning "Conversion from" }
        if (modulo(10_4, b8).ne.10_8) stop 51
        if (kind(modulo(10_4, b8)).ne.8) stop 52
        if (modulo(10_4, 5_8).ne.0_8) stop 53
        if (kind(modulo(10_4, 3_8)).ne.8) stop 54
        !!
        if (modulo(a8, b1).ne.3_8) stop 55       ! { dg-warning "Conversion from" }
        if (kind(modulo(a8, b1)).ne.8) stop 56   ! { dg-warning "Conversion from" }
        if (modulo(10_8, b1).ne.3_8) stop 57     ! { dg-warning "Conversion from" }
        if (kind(modulo(10_8, b1)).ne.8) stop 58 ! { dg-warning "Conversion from" }
        if (modulo(10_8, 5_1).ne.0_8) stop 59
        if (kind(modulo(10_8, 2_1)).ne.8) stop 60
        !
        if (modulo(a8, b2).ne.0_4) stop 61      ! { dg-warning "Conversion from" }
        if (kind(modulo(a8, b2)).ne.8) stop 62  ! { dg-warning "Conversion from" }
        if (modulo(10_8, b2).ne.2_8) stop 63    ! { dg-warning "Conversion from" }
        if (kind(modulo(1_8, b2)).ne.8) stop 64 ! { dg-warning "Conversion from" }
        if (modulo(10_8, 5_2).ne.0_8) stop 65
        if (kind(modulo(10_8, 12_2)).ne.8) stop 66
        !
        if (modulo(a8, b4).ne.8_8) stop 67      ! { dg-warning "Conversion from" }
        if (kind(modulo(a8, b4)).ne.8) stop 68  ! { dg-warning "Conversion from" }
        if (modulo(10_8, b4).ne.1_8) stop 69    ! { dg-warning "Conversion from" }
        if (kind(modulo(1_8, b4)).ne.8) stop 70 ! { dg-warning "Conversion from" }
        if (modulo(10_8, 5_4).ne.0_8) stop 71
        if (kind(modulo(10_8, 9_4)).ne.8) stop 72
        ! Mix types - check result is a real
        if (modulo(r4, b1).ne.4.0_4) stop 73   ! { dg-error "'a' and 'p' arguments" }
        if (kind(modulo(r4, b1)).ne.4) stop 74 ! { dg-error "'a' and 'p' arguments" }
        write(buffer,*) modulo(r4, b1)         ! { dg-error "'a' and 'p' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."4.0") stop 75
        if (modulo(a1, r4).ne.0.0_4) stop 76   ! { dg-error "'a' and 'p' arguments" }
        if (kind(modulo(a1, r4)).ne.4) stop 77 ! { dg-error "'a' and 'p' arguments" }
        write(buffer,*) modulo(a1, r4)         ! { dg-error "'a' and 'p' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."0.0") stop 78
	!
        if (modulo(r8, b2).ne.0.0_8) stop 79   ! { dg-error "'a' and 'p' arguments" }
        if (kind(modulo(r8, b2)).ne.8) stop 80 ! { dg-error "'a' and 'p' arguments" }
        write(buffer,*) modulo(r8, b2)         ! { dg-error "'a' and 'p' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."0.0") stop 81
        if (modulo(a2, r8).ne.4.0_8) stop 82   ! { dg-error "'a' and 'p' arguments" }
        if (kind(modulo(a2, r8)).ne.8) stop 83 ! { dg-error "'a' and 'p' arguments" }
        write(buffer,*) modulo(a2, r8)         ! { dg-error "'a' and 'p' arguments" }
        buffer = adjustl(buffer)
        if (buffer(1:3).ne."4.0") stop 84
      end program
