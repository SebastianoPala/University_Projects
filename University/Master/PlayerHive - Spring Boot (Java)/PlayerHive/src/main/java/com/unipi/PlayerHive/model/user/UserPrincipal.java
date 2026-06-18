package com.unipi.PlayerHive.model.user;

import java.util.Collection;
import java.util.Collections;

import lombok.Getter;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;

@Getter
public class UserPrincipal implements UserDetails {
    private final User user;
    public UserPrincipal(User user){
        this.user = user;
    }
    @Override
    public Collection<? extends GrantedAuthority> getAuthorities(){
        return Collections.singleton(() -> "ROLE_" + user.getRole().toUpperCase());
    }

    @Override 
    public String getPassword() {
        return user.getPassword();
    }

    @Override
    public String getUsername() {
        return user.getEmail(); // Spring uses email as "username" because email is our unique identifier
    }

    @Override 
    public boolean isAccountNonExpired(){
        return true;
    }
    @Override 
    public boolean isAccountNonLocked(){
        return true;
    }
    @Override 
    public boolean isCredentialsNonExpired(){
        return true;
    }
    @Override 
    public boolean isEnabled(){
        return true;
    }
}
