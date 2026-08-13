pkgname=livefetch
pkgver=2.2.0
pkgrel=1
pkgdesc="TUI system information tool with live-updating modules"
arch=('x86_64' 'aarch64')
url="https://github.com/WilGulf/livefetch"
license=('MIT')

source=("https://github.com/WilGulf/livefetch/archive/refs/tags/v{pkgver}.tar.gz")
sha256sums=('SKIP')

makedepends=('ncurses')

build() {
    make PREFIX="$pkgdir/usr"
}

package() {
    make install PREFIX="$pkgdir/usr"
}