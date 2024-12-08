#pragma once

namespace spotify_volume_controller
{

using keycode = unsigned int;
using volume_t = unsigned int;

struct volume
{
  volume(volume_t v)
      : m_volume(v)
  {
  }

  volume_t m_volume;

  [[nodiscard]] volume_t operator-(const volume other) const
  {
    return m_volume < other.m_volume ? 0 : m_volume - other.m_volume;
  }

  [[nodiscard]] volume_t operator+(const volume other) const
  {
    return m_volume + other.m_volume > 100 ? 100 : m_volume + other.m_volume;
  }

  volume& operator++()
  {
    if (m_volume < 100) {
      m_volume++;
    }
    return *this;
  }

  volume operator++(int)
  {
    volume old_val = *this;
    operator++();
    return old_val;
  }

  // prefix decrement
  volume& operator--()
  {
    // actual decrement takes place here
    if (m_volume > 1) {
      m_volume--;
    }
    return *this;  // return new value by reference
  }

  volume operator--(int)
  {
    volume old = *this;
    operator--();
    return old;
  }

  operator volume_t() const { return m_volume; }
};

}  // namespace spotify_volume_controller