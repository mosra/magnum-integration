Name: magnum-integration
Version: 2020.06.260.g343680a
Release: 1
Summary: Integration libraries for the Magnum C++11/C++14 graphics engine
License: MIT
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
Requires: magnum, bullet, bullet-extras, eigen3
BuildRequires: cmake, git, gcc-c++, bullet-devel, eigen3-devel, glm-devel
Source1: https://github.com/ocornut/imgui/archive/v1.88.zip

%description
Here are integration libraries for the Magnum C++11 graphics engine, providing
integration of various math and physics libraries into the engine itself.

%package devel
Summary: Magnum Integration development files
Requires: %{name} = %{version}, magnum-devel

%description devel
Headers and tools needed for integrating Magnum with various math and physics
libraries.

%prep
%setup -c -n %{name}-%{version}

%build
unzip %{SOURCE1} -d %{_builddir}

mkdir build && cd build
# Configure CMake
cmake ../%{name}-%{version} \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=%{_prefix} \
  -DIMGUI_DIR=%{_builddir}/imgui-1.88 \
  -DMAGNUM_WITH_BULLETINTEGRATION=ON \
  -DMAGNUM_WITH_DARTINTEGRATION=OFF \
  -DMAGNUM_WITH_EIGENINTEGRATION=ON \
  -DMAGNUM_WITH_GLMINTEGRATION=ON \
  -DMAGNUM_WITH_IMGUIINTEGRATION=ON \
  -DMAGNUM_WITH_YOGAINTEGRATION=OFF

make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
cd build
make DESTDIR=$RPM_BUILD_ROOT install
strip $RPM_BUILD_ROOT/%{_libdir}/*.so*

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{_builddir}/imgui-1.88

%files
%defattr(-,root,root,-)
%{_libdir}/*.so*

%doc %{name}-%{version}/COPYING

%files devel
%defattr(-,root,root,-)
%{_includedir}/Magnum
%{_datadir}/cmake/MagnumIntegration

%changelog
# TODO: changelog
