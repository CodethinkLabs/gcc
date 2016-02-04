! { dg-do compile }
! { dg-options "-fdec -fno-dec-hollerith-conversion" }

program convert
  integer*4 b
  integer*4 abcd
  logical bigendian
  abcd = ichar ("a")
  abcd = 256 * abcd + ichar ("b")
  abcd = 256 * abcd + ichar ("c")
  abcd = 256 * abcd + ichar ("d")
  bigendian = transfer (abcd, "wxyz") .eq. "abcd"
  ! Note: 5HRIVET will be truncated to "RIVE" and then converted to integer
  b = 5HRIVET ! { dg-warning "too long to convert" }
  if (4HJMAC.eq.400) stop 1 ! { dg-error "Operands of comparison" }
  if (bigendian) then
    if (4HRIVE.ne.1380537925) stop 2 ! { dg-error "Operands of comparison" }
  else
    if (4HRIVE.ne.1163282770) stop 2 ! { dg-error "Operands of comparison" }
  end if
  if (b.ne.4HRIVE) stop 3            ! { dg-error "Operands of comparison" }
  if (bigendian) then
    if (1380537925.ne.4HRIVE) stop 4 ! { dg-error "Operands of comparison" }
  else
    if (1163282770.ne.4HRIVE) stop 4 ! { dg-error "Operands of comparison" }
  end if
 end program

