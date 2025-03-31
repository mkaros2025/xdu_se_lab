// src/store/modules/auth.js
import apiClient from '@/services/api'
import router from '@/router'

const state = {
  user: null,
  userId: localStorage.getItem('userId') || null,
  role: localStorage.getItem('role') || null, // 添加角色
}

const mutations = {
  SET_USER(state, user) {
    state.user = user
  },
  SET_USER_ID(state, userId) {
    state.userId = userId
  },
  SET_ROLE(state, role) {
    state.role = role
  },
  CLEAR_AUTH(state) {
    state.user = null
    state.userId = null
    state.role = null
  },
}

// actions 可以通过 commit 提交突变
const actions = {
  // 用户注册
  async register(_, user) {
    // 注册成功后，用户需要手动登录
    // 返回的是用户 id
    return apiClient.post('/users/register', user)
  },

  // 用户登录
  async login({ commit, dispatch }, credentials) {
    const response = await apiClient.post('/users/login', credentials)
    // 后端返回 { message, user_id, role }
    const { user_id, role } = response.data
    commit('SET_USER_ID', user_id)
    commit('SET_ROLE', role)
    localStorage.setItem('userId', user_id)
    localStorage.setItem('role', role) // 存储角色
    // 获取用户信息
    await dispatch('fetchUser')
  },

  // 用户登出
  logout({ commit }) {
    commit('CLEAR_AUTH')
    localStorage.removeItem('userId')
    localStorage.removeItem('role') // 清除角色
    router.push('/login')  // 登出后跳转到登录页面
  },

  // 获取当前用户信息
  async fetchUser({ commit, state }) {
    try {
      if (state.userId) {
        const response = await apiClient.get(`/users/${state.userId}`)
        commit('SET_USER', response.data)
      }
    } catch (error) {
      commit('CLEAR_AUTH')
      throw error
    }
  },
}

// 获取器
const getters = {
  // 基于 userId 的存在与否返回布尔值，指示用户是否已认证
  isAuthenticated: (state) => !!state.userId,
  getUser: (state) => state.user,
  userId: (state) => state.userId,
  userRole: (state) => state.role, // 添加 userRole getter
}

export default {
  namespaced: true,
  state,
  mutations,
  actions,
  getters,
}

