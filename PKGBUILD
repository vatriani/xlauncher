#Maintainer: Niels Neumann <vatriani[dot]nn[at]googlemail[dot]com>
pkgname=xlauncher-git
pkgdesc="simple and fast launcher for x11"
pkgver=0.1.0
pkgrel=1
url="https://github.com/vatriani/xlauncher"
arch=any
license=('GPLv3')
depends=('libx11')

source=("https://github.com/vatriani/xlauncher/archive/master.zip")

md5sums=('ce1986e089aa06fe4b6a9608714be6f8')

package() {
	cd ${srcdir}/xlauncher-master
	make
	install -Dm777 xlauncher "$pkgdir/usr/bin/xlauncher"
}
