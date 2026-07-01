package com.unipi.PlayerHive.DTO.users;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.springframework.data.annotation.Id;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class GameOwnerDTO {
    @Id
    private String id;

    // relationship info
    private Double hoursPlayed;
}
