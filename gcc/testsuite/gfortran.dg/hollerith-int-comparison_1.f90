! { dg-do run }
! { dg-options "-fdec" }

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
  if (4HJMAC.eq.400) stop 1 ! { dg-warning "Promoting argument for" }
  if (bigendian) then
    if (4HRIVE.ne.1380537925) stop 2 ! { dg-warning "Promoting argument for" }
  else
    if (4HRIVE.ne.1163282770) stop 2 ! { dg-warning "Promoting argument for" }
  end if
  if (b.ne.4HRIVE) stop 3            ! { dg-warning "Promoting argument for" }
  if (bigendian) then
    if (1380537925.ne.4HRIVE) stop 4 ! { dg-warning "Promoting argument for" }
  else
    if (1163282770.ne.4HRIVE) stop 4 ! { dg-warning "Promoting argument for" }
  end if
 end program

