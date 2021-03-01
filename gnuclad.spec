Summary: cladogram generator
Name: gnuclad
Version: 0.2.4
Release: 1
URL: https://launchpad.net/gnuclad
Source0: %{name}-%{version}.tar.gz
License: GPLv3
Group: Development/Tools
BuildRoot: %{_tmppath}/%{name}-root

%description
gnuclad tries to help the environment by creating trees.
It's primary use will be generating cladogram trees for the GNU/Linux distro timeline project.


%prep
%setup -q
%configure 
%build
make
%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
rm $RPM_BUILD_ROOT/%{_infodir}/dir   # will not get packaged
install -m 644 -D doc/man/gnuclad.1 $RPM_BUILD_ROOT/%{_mandir}/man1/gnuclad.1  # not installed by Makefile?

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/gnuclad
%{_infodir}/gnuclad.info.gz
%{_mandir}/man1/gnuclad*
%doc example/ AUTHORS COPYING INSTALL NEWS README

%post
/sbin/install-info %{_infodir}/gnuclad.info.gz %{_infodir}/dir &> /dev/null 
:
%preun
if [ $1 = 0 ]; then
  /sbin/install-info --delete %{_infodir}/gnuclad.info.gz %{_infodir}/dir &> /dev/null :
fi
:

%changelog
* Thu Jun 20 2013 Jan Iven <jan.iven@cern.ch>
- Initial build.


