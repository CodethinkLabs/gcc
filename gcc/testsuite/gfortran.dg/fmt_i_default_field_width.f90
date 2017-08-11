! { dg-do run }
! { dg-options -std=extra-legacy }
!
! Test case for the default field widths enabled by the --std=extra-legacy flag.
!
! This feature is not part of any Fortran standard, but it is supported by the
! Oracle Fortran compiler and others.

    character(50) :: buffer

    integer*2 :: integer_2
    integer*8 :: integer_8

    write(buffer, '(A, I, A)') ':',12340,':'
    print *,buffer
    if (buffer.ne.":       12340:") call abort

    integer_2 = -99
    write(buffer, '(A, I, A)') ':',integer_2,':'
    print *,buffer
    if (buffer.ne.":    -99:") call abort

    integer_8 = -11112222
    write(buffer, '(A, I, A)') ':',integer_8,':'
    print *,buffer
    if (buffer.ne.":              -11112222:") call abort
end
