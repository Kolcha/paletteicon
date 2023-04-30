/*
    custom icon engine Qt plugin
    Copyright (C) 2017-2020  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "palette_icon_engine.h"

#include <QFile>
#include <QPainter>
#include <QPalette>
#include <QPixmapCache>
#include <QSvgRenderer>

static QString actualFilename(const QString& filename)
{
  if (QFile::exists(filename))
    return filename;

  QString fn = filename.mid(0, filename.lastIndexOf('.'));
  if (QFile::exists(fn))
    return fn;

  return fn + ".svg";
}

static QColor getIconColor(QIcon::Mode mode, QIcon::State state)
{
  Q_UNUSED(state);
  QPalette::ColorGroup color_group = QPalette::Active;
  if (mode == QIcon::Disabled) color_group = QPalette::Disabled;
  return QPalette().color(color_group, QPalette::WindowText);
}

static QPixmap renderIcon(QSvgRenderer* renderer, const QSize& size, const QBrush& brush)
{
  QPixmap output(size);
  output.fill(Qt::transparent);

  QPainter p(&output);
  renderer->render(&p);

  p.setCompositionMode(QPainter::CompositionMode_SourceIn);

  p.setPen(Qt::NoPen);
  p.setBrush(brush);

  p.drawRect(output.rect());

  return output;
}


PaletteIconEngine::PaletteIconEngine()
{
  renderer_.reset(new QSvgRenderer());
}

PaletteIconEngine::PaletteIconEngine(const PaletteIconEngine& other) : QIconEngine(other)
{
  src_file_ = other.src_file_;
  renderer_.reset(new QSvgRenderer());
  if (other.renderer_->isValid())
    renderer_->load(other.src_file_);
}

void PaletteIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
  // "direct rendereng" using given painter is not possible
  // because colorization logic modifies already painted area
  // such behavior is not acceptable, so render icon to pixmap first
  QColor color = getIconColor(mode, state);
  QPixmap out = renderIcon(renderer_.data(), rect.size() * painter->device()->devicePixelRatioF(), color);
  out.setDevicePixelRatio(painter->device()->devicePixelRatioF());
  painter->drawPixmap(rect, out);
}

QPixmap PaletteIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
  QColor color = getIconColor(mode, state);

  QString pmckey = QString("pie_%1:%2x%3:%4-%5%6")
                   .arg(src_file_)
                   .arg(size.width()).arg(size.height())
                   .arg(mode)
                   .arg(state)
                   .arg(color.name(QColor::HexArgb));

  QPixmap pxm;
  if (!QPixmapCache::find(pmckey, &pxm)) {
    pxm = renderIcon(renderer_.data(), size, color);
    QPixmapCache::insert(pmckey, pxm);
  }
  return pxm;
}

void PaletteIconEngine::addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state)
{
  Q_UNUSED(size);
  Q_UNUSED(mode);
  Q_UNUSED(state);
  QString filename = actualFilename(fileName);
  if (filename == src_file_)
    return;
  if (renderer_->load(filename))
    src_file_ = filename;
}

QIconEngine* PaletteIconEngine::clone() const
{
  return new PaletteIconEngine(*this);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QList<QSize> PaletteIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state) const
#else
QList<QSize> PaletteIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state)
#endif
{
  Q_UNUSED(mode);
  Q_UNUSED(state);
  QList<QSize> sizes;
  sizes << QSize(512, 512);     // just workaround to make tray icon visible on KDE
  return sizes;
}

void PaletteIconEngine::virtual_hook(int id, void* data)
{
  switch (id) {
    case QIconEngine::IsNullHook:
      *reinterpret_cast<bool*>(data) = (!renderer_ || !renderer_->isValid());
      break;

    default:
      QIconEngine::virtual_hook(id, data);
  }
}
