<!-- src/components/Auth/UserRegister.vue -->

<template>
  <v-container>
    <v-card class="mx-auto small-height-card" max-width="500">
      <v-card-title class="card-title">
        <span class="headline">注册</span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" v-model="valid" @submit.prevent="submit">
          <!-- 用户名输入框 -->
          <v-text-field v-model="username" label="用户名" :rules="[v => !!v || '用户名为必填项']" required dense
            class="form-field" outlined></v-text-field>

          <!-- 密码输入框 -->
          <v-text-field v-model="password" label="密码" type="password" :rules="[v => !!v || '密码为必填项']" required dense
            class="form-field" outlined></v-text-field>

          <v-select v-model="role" :items="roles" label="角色" :rules="[v => !!v || '角色为必填项']" required dense
            class="form-field" outlined></v-select>

          <!-- 姓名输入框 -->
          <v-text-field v-model="name" label="姓名" dense class="form-field" outlined></v-text-field>

          <!-- 电话号码输入框 -->
          <v-text-field v-model="phone_number" label="电话号码" type="tel" dense class="form-field" outlined></v-text-field>

          <!-- 地址文本区域 -->
          <v-textarea v-model="address" label="地址" dense class="form-field" outlined></v-textarea>

          <!-- 提交按钮 -->
          <v-btn :disabled="!valid || loading" color="primary" type="submit" class="rounded-button">
            注册
          </v-btn>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn text @click="$router.push('/login')">已有账户？登录</v-btn>
      </v-card-actions>
    </v-card>

    <!-- 显示消息 -->
    <v-snackbar v-model="snackbar" :color="snackbarColor" timeout="1000">
      {{ snackbarMessage }}
      <template v-slot:actions>
        <v-btn
          color="white"
          variant="text"
          @click="snackbar = false"
        >
          Close
        </v-btn>
      </template>
    </v-snackbar>
  </v-container>
</template>

<script>
import { mapActions } from 'vuex'

export default {
  name: 'UserRegister',
  data() {
    return {
      valid: false,
      username: '',
      password: '',
      role: '',
      roles: ['customer', 'staff', 'delivery'], // 根据后端定义的角色
      name: '',
      phone_number: '',
      address: '',
      loading: false,
      snackbar: false,
      snackbarMessage: '',
      snackbarColor: '',
    }
  },
  methods: {
    ...mapActions('auth', ['register']),
    async submit() {
      if (this.$refs.form.validate()) {
        this.loading = true
        try {
          const user = {
            username: this.username,
            password: this.password,
            role: this.role,
            name: this.name,
            phone_number: this.phone_number,
            address: this.address,
          }
          // 注册用户
          await this.register(user)
          this.snackbarMessage = '注册成功！请登录。'
          this.snackbarColor = 'success'
          this.snackbar = true
          // 清空表单
          this.username = ''
          this.password = ''
          this.role = ''
          this.name = ''
          this.phone_number = ''
          this.address = ''
          // 跳转到登录页面，延迟以确保用户看到 Snackbar
          setTimeout(() => {
            this.$router.push('/login')
          }, 500)
        } catch (error) {
          this.snackbarMessage = error.response?.data?.message || '注册失败，请重试。'
          this.snackbarColor = 'error'
          this.snackbar = true
          console.error('注册失败：', error)
        } finally {
          this.loading = false
        }
      }
    },
  },
}
</script>

<style scoped>
.rounded-button {
  width: 100%;
  /* 按钮全宽 */
  margin-top: 6px;
  /* 增加按钮与上方输入框的间距 */
}

/* 缩小卡片的整体高度 */
.small-height-card {
  padding-top: 0px;
  /* 调整顶部填充 */
  padding-bottom: -10px;
  /* 调整底部填充 */
}

/* 缩小表单字段之间的间距 */
.form-field {
  margin-bottom: 5px;
  /* 调整为较小的间距 */
}

/* 可选：减少卡片标题和表单之间的间距 */
.card-title {
  margin-bottom: 0px;
  /* 调整为较小的间距 */
}
</style>
