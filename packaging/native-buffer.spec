%define _include_tc 0

%define _prefix		/usr
%define _libdir		%{_prefix}/lib

Name:           native-buffer
Version:        1.1.3
Release:        2
License:        MIT and Apache-2.0
Summary:        Native Buffer library project
Group:          System/Libraries
Source0:        %{name}-%{version}.tar.gz
Source1001: 	packaging/native-buffer.manifest

BuildRequires: cmake
BuildRequires: pkgconfig(libdrm)
BuildRequires: pkgconfig(libtbm)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(xproto)
BuildRequires: pkgconfig(libdri2)
BuildRequires: pkgconfig(xfixes)
BuildRequires: pkgconfig(dlog)
%if 0%{?_include_tc}
BuildRequires: pkgconfig(turbojpeg)
%endif
BuildRequires: model-build-features

%description

%package -n native-buffer-devel
Summary:	Development components for the Native Buffer library
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}

%description -n native-buffer-devel
native buffer development libraries and head files

%package -n native-buffer-internal-devel
Summary:	Development components for the Native Buffer library for internal use
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}

%description -n native-buffer-internal-devel
native buffer internal development libraries and head files

%if 0%{?_include_tc}
%package -n native-buffer-tc
Summary:	Test case for the Native Buffer library
Group:		Development/Libraries
Requires:	%{name} = %{version}-%{release}
BuildRequires: pkgconfig(opengl-es-20)

%description -n native-buffer-tc
native buffer test cases
%endif

%prep
%setup -q

%build
%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

CFLAGS+=" -fvisibility=hidden -fdata-sections -ffunction-sections -Wl,--gc-sections"
cp %{SOURCE1001} .

%if 0%{?_include_tc}
%global extra_option -DBUILD_TC=TRUE
%endif

%if "%{?model_build_feature_graphics_gpu_info}" == "mali400"
%global extra_option -DUSE_MALI=TRUE
%endif

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_LIB_DIR=%{_libdir} -DPACKAGE_VERSION=%{version} %{?extra_option}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp -af COPYING %{buildroot}/usr/share/license/%{name}
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%manifest native-buffer.manifest
%{_libdir}/libnative-buffer.so*
/usr/share/license/%{name}

%files -n native-buffer-devel
%{_includedir}/native-buffer.h
%{_libdir}/libnative-buffer.so
%{_libdir}/pkgconfig/native-buffer.pc

%files -n native-buffer-internal-devel
%{_includedir}/native-buffer-private.h

%if 0%{?_include_tc}
%files -n native-buffer-tc
%{_bindir}/*
%endif

