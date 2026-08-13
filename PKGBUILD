pkgname=livefetch
pkgver=2.2.0
pkgrel=1
pkgdesc="TUI system information tool with live-updating modules"
arch=('x86_64' 'aarch64')
url="https://github.com/WilGulf/livefetch"
license=('MIT')

source=("https://github.com/WilGulf/livefetch/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('SKIP')

makedepends=('ncurses')
depends=('ncurses')

build() {
    cd "$srcdir/livefetch-${pkgver}"
    make
}

package() {
    cd "$srcdir/livefetch-${pkgver}"
    make install PREFIX="$pkgdir/usr"
}