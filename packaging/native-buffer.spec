%define _include_tc 0

%define _prefix		/usr
%define _libdir		%{_prefix}/lib

Name:           native-buffer
Version:        1.1.0
Release:        1
VCS:            magnolia/framework/graphics/native-buffer#6fbba03a5c22123f54bfc60a181cd99872bd0a8b
License:        MIT
Summary:        Native Buffer library project
Group:          System/Libraries
ExcludeArch:    i586
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

%global extra_option -DUSE_MALI=TRUE

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_LIB_DIR=%{_libdir} -DPACKAGE_VERSION=%{version} %{?extra_option}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%manifest native-buffer.manifest
%{_libdir}/libnative-buffer.so*

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

