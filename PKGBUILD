# Maintainer: Benno Straub <benno.straub@outlook.de>
pkgname=wwidgets-git # '-bzr', '-git', '-hg' or '-svn'
pkgver=0.1
pkgrel=1
pkgdesc="A widget library, esp. for games"
arch=("x86" "x86_64" "ARM")
url="www.github.com/Cannedfood/WonkyWidgets"
license=('GPL')
groups=()
depends=('glfw')
makedepends=('git') # 'bzr', 'git', 'mercurial' or 'subversion'
provides=("wwidgets")
conflicts=("wwidgets")
replaces=()
backup=()
options=()
install=
source=("${pkgname%-git}::git+https://github.com/Cannedfood/WonkyWidgets")
noextract=()
md5sums=('SKIP')

pkgver() {
	cd "$srcdir/${pkgname%-git}"
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd "$srcdir/${pkgname%-git}"
	c++ --std=c++17 -O2 -lglfw -shared -fPIC -o "lib${pkgname%-git}.so" $(find  "src/" -name *.cpp)
}

package() {
	cd "$srcdir/${pkgname%-git}"
	mkdir -p "$pkgdir/usr/lib/"
	cp "lib${pkgname%-git}.so" "$pkgdir/usr/lib/"
	cp -r "include/" "$pkgdir/usr/include/"
}
