#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace spotify_volume_controller
{

using keycode = unsigned int;
using volume_t = unsigned int;

constexpr volume_t max_volume {100};

struct volume
{
  volume(const volume&) = default;
  volume(volume&&) = default;
  volume& operator=(const volume&) = default;
  volume& operator=(volume&&) = default;
  ~volume() = default;
  volume() = default;

  explicit volume(volume_t vol)
      : m_volume(vol)
  {
  }

  volume_t m_volume;

  [[nodiscard]] volume operator-(const volume other) const
  {
    return volume(m_volume < other.m_volume ? 0 : m_volume - other.m_volume);
  }

  [[nodiscard]] volume operator+(const volume other) const
  {
    return volume(m_volume + other.m_volume > max_volume ? max_volume : m_volume + other.m_volume);
  }

  volume& operator++()
  {
    if (m_volume < max_volume) {
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

  explicit operator volume_t() const { return m_volume; }
};

struct token_t
{
  std::string access_token;
  std::string token_type;
  std::string scope;
  std::int64_t expires_in;
  std::string refresh_token;
  std::int64_t expires_at;

  explicit token_t(nlohmann::json j_token)
      : access_token {j_token["access_token"].template get<std::string>()}
      , token_type {j_token["token_type"].template get<std::string>()}
      , scope {j_token["scope"].template get<std::string>()}
      , expires_in {j_token["expires_in"].template get<std::int64_t>()}
      , refresh_token {j_token["refresh_token"].is_string() ? j_token["refresh_token"].template get<std::string>() : ""}
      , expires_at {j_token.contains("expires_at") ? j_token["expires_at"].template get<std::int64_t>() : 0}
  {
  }

  [[nodiscard]] nlohmann::json as_json() const
  {
    nlohmann::json j_token {};
    j_token["access_token"] = access_token;
    j_token["token_type"] = token_type;
    j_token["scope"] = scope;
    j_token["expires_in"] = expires_in;
    j_token["refresh_token"] = refresh_token;
    j_token["expires_at"] = expires_at;
    return j_token;
  }
};

}  // namespace spotify_volume_controller